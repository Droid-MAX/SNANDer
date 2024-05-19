/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2011 asbokid <ballymunboy@gmail.com>
 * Copyright (C) 2014 Pluto Yang <yangyj.ee@gmail.com>
 * Copyright (C) 2015-2016 Stefan Tauner
 * Copyright (C) 2015 Urja Rannikko <urjaman@gmail.com>
 * Copyright (C) 2018-2021 McMCC <mcmcc@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <string.h>
#include <stdio.h>
#include "ch347_spi.h"
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
/* LIBUSB_CALL ensures the right calling conventions on libusb callbacks.
 * However, the macro is not defined everywhere. m(
 */
#ifndef LIBUSB_CALL
#define LIBUSB_CALL
#endif

#define CH347_VID 0x1A86
#define CH347T_PID 0x55DB
#define CH347F_PID 0x55DE

#define CH347_CMD_SPI_SET_CFG	0xC0
#define CH347_CMD_SPI_CS_CTRL	0xC1
#define CH347_CMD_SPI_OUT_IN	0xC2
#define CH347_CMD_SPI_IN	0xC3
#define CH347_CMD_SPI_OUT	0xC4
#define CH347_CMD_SPI_GET_CFG	0xCA

#define CH347_CS_ASSERT		0x00
#define CH347_CS_DEASSERT	0x40
#define CH347_CS_CHANGE		0x80
#define CH347_CS_IGNORE		0x00

#define WRITE_EP	0x06
#define READ_EP 	0x86

#define MODE_1_IFACE 2
#define MODE_2_IFACE 1


#define	 CH341_PACKET_LENGTH		0x20


struct device_speeds {
	const char *name;
	const int speed;
};
/* The USB descriptor says the max transfer size is 512 bytes, but the
 * vendor driver only seems to transfer a maximum of 510 bytes at once,
 * leaving 507 bytes for data as the command + length take up 3 bytes
 */
#define CH347_PACKET_SIZE 510
#define CH347_MAX_DATA_LEN (CH347_PACKET_SIZE - 3)
static const struct device_speeds spispeeds[] = {
	{"60M",		0x0},
	{"30M",		0x1},
	{"15M",		0x2},
	{"7.5M",	0x3},
	{"3.75M",	0x4},
	{"1.875M",	0x5},
	{"937.5K",	0x6},
	{"468.75",	0x7},
	{NULL,		0x0}
};

#ifdef _WIN32
#include <windows.h>
typedef int(__stdcall  * pCH347OpenDevice)(unsigned long iIndex);

typedef int(__stdcall * pCH347CloseDevice)(unsigned long iIndex);
typedef unsigned long(__stdcall * pCH347SetTimeout)(
	unsigned long iIndex,        /* Specify equipment serial number */
	unsigned long iWriteTimeout, /* Specifies the timeout period for USB
					write out data blocks, in milliseconds
					mS, and 0xFFFFFFFF specifies no timeout
					(default) */
	unsigned long iReadTimeout); /* Specifies the timeout period for USB
					reading data blocks, in milliseconds mS,
					and 0xFFFFFFFF specifies no timeout
					(default) */

typedef unsigned long(__stdcall * pCH347WriteData)(
	unsigned long iIndex,         /* Specify equipment serial number */
	void *oBuffer,                /* Point to a buffer large enough to hold
					 the descriptor */
	unsigned long *ioLength);     /* Pointing to the length unit, the input
					 is the length to be read, and the
					 return is the actual read length */

typedef unsigned long(__stdcall * pCH347ReadData)(
	unsigned long iIndex,          /* Specify equipment serial number */
	void *oBuffer,                 /* Point to a buffer large enough to
					  hold the descriptor */
	unsigned long *ioLength);      /* Pointing to the length unit, the input
					  is the length to be read, and the
					  return is the actual read length */
HMODULE uhModule = 0;
ULONG ugIndex = -1;
pCH347OpenDevice CH347OpenDevice;
pCH347CloseDevice CH347CloseDevice;
pCH347SetTimeout CH347SetTimeout;
pCH347ReadData CH347ReadData;
pCH347WriteData CH347WriteData;
BOOL DevIsOpened = FALSE; /* Whether the device is turned on */
#endif
struct libusb_device_handle *devHandle;

/* Number of parallel IN transfers. 32 seems to produce the most stable throughput on Windows. */
#define USB_IN_TRANSFERS		32

struct dev_entry {
	uint16_t vendor_id;
	uint16_t device_id;
	const char *vendor_name;
	const char *device_name;
};

/* TODO: Add support for HID mode */
static const struct dev_entry devs_ch347_spi[] = {
	{CH347_VID, CH347T_PID, "WCH", "CH347T"},
	{CH347_VID, CH347F_PID, "WCH", "CH347F"},
	{0},
};

/* We need to use many queued IN transfers for any resemblance of performance (especially on Windows)
 * because USB spec says that transfers end on non-full packets and the device sends the 31 reply
 * data bytes to each 32-byte packet with command + 31 bytes of data... */
static struct libusb_transfer *transfer_out = NULL;
static struct libusb_transfer *transfer_ins[USB_IN_TRANSFERS] = {0};
struct libusb_device_handle *handle = NULL;

enum trans_state {TRANS_ACTIVE = -2, TRANS_ERR = -1, TRANS_IDLE = 0};

#if 0
static void print_hex(const void *buf, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++) {
		printf(" %02x", ((uint8_t *)buf)[i]);
		if (i % CH341_PACKET_LENGTH == CH341_PACKET_LENGTH - 1)
			printf("\n");
	}
}


static void cb_common(const char *func, struct libusb_transfer *transfer)
{
	int *transfer_cnt = (int*)transfer->user_data;

	if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
		/* Silently ACK and exit. */
		*transfer_cnt = TRANS_IDLE;
		return;
	}

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
		printf("\n%s: error: %s\n", func, libusb_error_name(transfer->status));
		*transfer_cnt = TRANS_ERR;
	} else {
		*transfer_cnt = transfer->actual_length;
	}
}

/* callback for bulk out async transfer */
static void LIBUSB_CALL cb_out(struct libusb_transfer *transfer)
{
	cb_common(__func__, transfer);
}

/* callback for bulk in async transfer */
static void LIBUSB_CALL cb_in(struct libusb_transfer *transfer)
{
	cb_common(__func__, transfer);
}

static int32_t usb_transfer(const char *func, unsigned int writecnt, unsigned int readcnt, const uint8_t *writearr, uint8_t *readarr)
{
	if (handle == NULL)
		return -1;

	int state_out = TRANS_IDLE;
	transfer_out->buffer = (uint8_t*)writearr;
	transfer_out->length = writecnt;
	transfer_out->user_data = &state_out;

	/* Schedule write first */
	if (writecnt > 0) {
		state_out = TRANS_ACTIVE;
		int ret = libusb_submit_transfer(transfer_out);
		if (ret) {
			printf("%s: failed to submit OUT transfer: %s\n", func, libusb_error_name(ret));
			state_out = TRANS_ERR;
			goto err;
		}
	}

	/* Handle all asynchronous packets as long as we have stuff to write or read. The write(s) simply need
	 * to complete but we need to scheduling reads as long as we are not done. */
	unsigned int free_idx = 0; /* The IN transfer we expect to be free next. */
	unsigned int in_idx = 0; /* The IN transfer we expect to be completed next. */
	unsigned int in_done = 0;
	unsigned int in_active = 0;
	unsigned int out_done = 0;
	uint8_t *in_buf = readarr;
	int state_in[USB_IN_TRANSFERS] = {0};
	do {
		/* Schedule new reads as long as there are free transfers and unscheduled bytes to read. */
		while ((in_done + in_active) < readcnt && state_in[free_idx] == TRANS_IDLE) {
			unsigned int cur_todo = min(CH341_PACKET_LENGTH - 1, readcnt - in_done - in_active);
			transfer_ins[free_idx]->length = cur_todo;
			transfer_ins[free_idx]->buffer = in_buf;
			transfer_ins[free_idx]->user_data = &state_in[free_idx];
			int ret = libusb_submit_transfer(transfer_ins[free_idx]);
			if (ret) {
				state_in[free_idx] = TRANS_ERR;
				printf("%s: failed to submit IN transfer: %s\n",
					 func, libusb_error_name(ret));
				goto err;
			}
			in_buf += cur_todo;
			in_active += cur_todo;
			state_in[free_idx] = TRANS_ACTIVE;
			free_idx = (free_idx + 1) % USB_IN_TRANSFERS; /* Increment (and wrap around). */
		}

		/* Actually get some work done. */
		libusb_handle_events_timeout(NULL, &(struct timeval){1, 0});

		/* Check for the write */
		if (out_done < writecnt) {
			if (state_out == TRANS_ERR) {
				goto err;
			} else if (state_out > 0) {
				out_done += state_out;
				state_out = TRANS_IDLE;
			}
		}
		/* Check for completed transfers. */
		while (state_in[in_idx] != TRANS_IDLE && state_in[in_idx] != TRANS_ACTIVE) {
			if (state_in[in_idx] == TRANS_ERR) {
				goto err;
			}
			/* If a transfer is done, record the number of bytes read and reuse it later. */
			in_done += state_in[in_idx];
			in_active -= state_in[in_idx];
			state_in[in_idx] = TRANS_IDLE;
			in_idx = (in_idx + 1) % USB_IN_TRANSFERS; /* Increment (and wrap around). */
		}
	} while ((out_done < writecnt) || (in_done < readcnt));
#if 0
	if (out_done > 0) {
		printf("Wrote %d bytes:\n", out_done);
		print_hex(writearr, out_done);
		printf("\n\n");
	}
	if (in_done > 0) {
		printf("Read %d bytes:\n", in_done);
		print_hex(readarr, in_done);
		printf("\n\n");
	}
#endif
	return 0;
err:
	/* Clean up on errors. */
	printf("%s: Failed to %s %d bytes\n", func, (state_out == TRANS_ERR) ? "write" : "read",
		 (state_out == TRANS_ERR) ? writecnt : readcnt);
	/* First, we must cancel any ongoing requests and wait for them to be canceled. */
	if ((writecnt > 0) && (state_out == TRANS_ACTIVE)) {
		if (libusb_cancel_transfer(transfer_out) != 0)
			state_out = TRANS_ERR;
	}
	if (readcnt > 0) {
		unsigned int i;
		for (i = 0; i < USB_IN_TRANSFERS; i++) {
			if (state_in[i] == TRANS_ACTIVE)
				if (libusb_cancel_transfer(transfer_ins[i]) != 0)
					state_in[i] = TRANS_ERR;
		}
	}

	/* Wait for cancellations to complete. */
	while (1) {
		bool finished = true;
		if ((writecnt > 0) && (state_out == TRANS_ACTIVE))
			finished = false;
		if (readcnt > 0) {
			unsigned int i;
			for (i = 0; i < USB_IN_TRANSFERS; i++) {
				if (state_in[i] == TRANS_ACTIVE)
					finished = false;
			}
		}
		if (finished)
			break;
		libusb_handle_events_timeout(NULL, &(struct timeval){1, 0});
	}
	return -1;
}
#endif

static uint16_t read_le16(const uint8_t *base, size_t offset) {
    // 读取 16 位值（两个字节）并以小端序合并
    uint16_t value = base[offset] | (base[offset + 1] << 8);
	// printf("value = %d\n",value);
    return value;
}

/*   Set the I2C bus speed (speed(b1b0): 0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz).
 *   Set the SPI bus data width (speed(b2): 0 = Single, 1 = Double).  */
int config_stream(unsigned int speed)
{
	int32_t ret;
	uint8_t buff[29] = {
		[0] = CH347_CMD_SPI_SET_CFG,
		[1] = (sizeof(buff) - 3) & 0xFF,
		[2] = ((sizeof(buff) - 3) & 0xFF00) >> 8,
		/* Not sure what these two bytes do, but the vendor
		 * drivers seem to unconditionally set these values
		 */
		[3] = 0,
		[4] = 0,
		[5] = 4,
		[6] = 1,
		/* Clock polarity: bit 1 */
		[9] = 0,
		/* Clock phase: bit 0 */
		[11] = 0,
		/* Another mystery byte */
		[14] = 2,
		/* Clock divisor: bits 5:3 */
		[15] = (speed & 0x7) << 3,
		/* Bit order: bit 7, 0=MSB */
		[17] = 0,
		/* Yet another mystery byte */
		[19] = 7,
		/* CS polarity: bit 7 CS2, bit 6 CS1. 0 = active low */
		[24] = 0
	};
	ULONG transferred = sizeof(buff);
#ifdef _WIN32
	if (!CH347WriteData(ugIndex, buff, &transferred)){
		printf("Could not configure SPI interface\n");
		return -1;
	}
	transferred = 4;
	if (!CH347ReadData(ugIndex, buff, &transferred) || buff[3] != 0x0){
		printf("configure SPI fail\n");
		return -1;
	}
	ret = 0;
#elif defined(_linux_)
	ret = libusb_bulk_transfer(ch347_data->handle, WRITE_EP, buff, sizeof(buff), NULL, 1000);
	if (ret < 0) {
		printf("Could not configure SPI interface\n");
	}

	/* FIXME: Not sure if the CH347 sends error responses for
	 * invalid config data, if so the code should check
	 */
	ret = libusb_bulk_transfer(ch347_data->handle, READ_EP, buff, 4, NULL, 1000);
	if (ret < 0 || buff[3] != 0x0) {
		printf("Could not receive configure SPI command response\n");
	}
#endif
	return ret;
}

/* ch341 requires LSB first, swap the bit order before send and after receive */
static uint8_t swap_byte(uint8_t x)
{
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
	x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
	return x;
}

static int ch347_cs_control(uint8_t cs1, uint8_t cs2)
{
	uint8_t cmd[13] = {
		[0] = CH347_CMD_SPI_CS_CTRL,
		/* payload length, uint16 LSB: 10 */
		[1] = 10,
		[3] = cs1,
		[8] = cs2
	};
	ULONG transferred = sizeof(cmd);
#ifdef _WIN32
	if (!CH347WriteData(ugIndex, cmd, &transferred) || transferred != sizeof(cmd)){
		printf("Could not change CS!\n");
		return -1;
	}
#elif defined(__linux__)
	int32_t ret = libusb_bulk_transfer(handle, WRITE_EP, cmd, sizeof(cmd), NULL, 1000);
	if (ret < 0) {
		printf("Could not change CS!\n");
		return -1;
	}
#endif
	return 0;
}

int enable_pins(bool enable)
{
	if (enable){
		ch347_cs_control(CH347_CS_ASSERT | CH347_CS_CHANGE, CH347_CS_IGNORE);
	}
	else{
		ch347_cs_control(CH347_CS_DEASSERT | CH347_CS_CHANGE, CH347_CS_IGNORE);
	}
}

static int ch347_write(unsigned int writecnt, const uint8_t *writearr)
{
	unsigned int data_len;
	int packet_len;
	ULONG transferred;
	int ret;
	uint8_t resp_buf[4] = {0};
	uint8_t buffer[CH347_PACKET_SIZE] = {0};
	unsigned int bytes_written = 0;

	while (bytes_written < writecnt) {
		data_len = min(CH347_MAX_DATA_LEN, writecnt - bytes_written );
		packet_len = data_len + 3;

		buffer[0] = CH347_CMD_SPI_OUT;
		buffer[1] = (data_len) & 0xFF;
		buffer[2] = ((data_len) & 0xFF00) >> 8;
		memcpy(buffer + 3, writearr + bytes_written, data_len);
#ifdef _WIN32
	transferred = packet_len;
	if (!CH347WriteData(ugIndex, buffer, &transferred) || transferred != packet_len){
		printf("Could not send write command\n");
		return -1;
	}
	transferred = sizeof(resp_buf);
	if(!CH347ReadData(ugIndex, resp_buf, &transferred) || transferred != sizeof(resp_buf)){
		printf("Could not receive write command response\n");
		return -1;
	}
#elif defined(__linux__)
    ret = libusb_bulk_transfer(handle, WRITE_EP, buffer, packet_len, &transferred, 1000);
	if (ret < 0 || transferred != packet_len) {
		printf("Could not send write command\n");
		return -1;
	}
	ret = libusb_bulk_transfer(handle, READ_EP, resp_buf, sizeof(resp_buf), NULL, 1000);
	if (ret < 0) {
		printf("Could not receive write command response\n");
		return -1;
	}
#endif
		bytes_written += data_len;
	}
	return 0;
}

static int ch347_read(unsigned int readcnt, uint8_t *readarr)
{
	uint8_t *read_ptr = readarr;
	int ret;
	ULONG transferred;
	unsigned int bytes_read = 0;
	uint8_t buffer[CH347_PACKET_SIZE] = {0};
	uint8_t command_buf[7] = {
		[0] = CH347_CMD_SPI_IN,
		[1] = 4,
		[2] = 0,
		[3] = readcnt & 0xFF,
		[4] = (readcnt & 0xFF00) >> 8,
		[5] = (readcnt & 0xFF0000) >> 16,
		[6] = (readcnt & 0xFF000000) >> 24
	};
#ifdef _WIN32
	transferred = sizeof(command_buf);
	if (!CH347WriteData(ugIndex, command_buf, &transferred) || transferred != sizeof(command_buf)){
		printf("Could not send read command\n");
		return -1;
	}
#elif defined(_linux_)
	ret = libusb_bulk_transfer(ch347_data->handle, WRITE_EP, command_buf, sizeof(command_buf), &transferred, 1000);
		if (ret < 0 || transferred != sizeof(command_buf)) {
			printf("Could not send read command\n");
			return -1;
		}
#endif
	while (bytes_read < readcnt) {
#ifdef _WIN32
	transferred = CH347_PACKET_SIZE;
	if (!CH347ReadData(ugIndex, buffer, &transferred)){
		printf("Could not read data\n");
		return -1;
	}
#elif defined(_linux_)
	ret = libusb_bulk_transfer(handle, READ_EP, buffer, CH347_PACKET_SIZE, &transferred, 1000);
	if (ret < 0) {
		printf("Could not read data\n");
		return -1;
	}
#endif
		if (transferred > CH347_PACKET_SIZE) {
			printf("libusb bug: bytes received overflowed buffer\n");
			return -1;
		}
		/* Response: u8 command, u16 data length, then the data that was read */
		if (transferred < 3) {
			printf("CH347 returned an invalid response to read command\n");
			return -1;
		}
		// printf("buffer [1] = %02x [2] = %02x\n",buffer[1], buffer[2]);
		int ch347_data_length = read_le16(buffer, 1);
		if (transferred - 3 < ch347_data_length) {
			printf("CH347 returned less data than data length header indicates\n");
			return -1;
		}
		bytes_read += ch347_data_length;
		if (bytes_read > readcnt) {
			printf("CH347 returned more bytes than requested\n");
			return -1;
		}
		memcpy(read_ptr, buffer + 3, ch347_data_length);
		read_ptr += ch347_data_length;
	}
	return 0;
}

int ch347_spi_send_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr)
{
	int ret;
	// ch347_cs_control(CH347_CS_ASSERT | CH347_CS_CHANGE, CH347_CS_IGNORE);
	if (writecnt) {
		ret = ch347_write(writecnt, writearr);
		if (ret < 0) {
			printf("CH347 write error\n");
			return -1;
		}
	}
	if (readcnt) {
		ret = ch347_read(readcnt, readarr);
		if (ret < 0) {
			printf("CH347 read error\n");
			return -1;
		}
	}
	// ch347_cs_control(CH347_CS_DEASSERT | CH347_CS_CHANGE, CH347_CS_IGNORE);
	return 0;
}

int ch347_spi_shutdown(void)
{
	if (handle == NULL)
		return -1;
#ifdef _WIN32
	if (CH347CloseDevice(ugIndex) == -1){
	    printf("Close the CH347 failed.\n");
    }else{
	    DevIsOpened = FALSE;
    }
#elif defined(__linux__)
	struct ch347_spi_data *ch347_data = data;
    /* TODO: Set this depending on the mode */
	int spi_interface = MODE_1_IFACE;
	libusb_release_interface(handle, spi_interface);
	libusb_attach_kernel_driver(handle, spi_interface);
	libusb_close(handle);
	libusb_exit(NULL);
#endif
	return 0;
}

int ch347_spi_init(void)
{
	int open_res = -1;
    uint16_t vid = CH347_VID;
	uint16_t pid = devs_ch347_spi[0].device_id;
	int spispeed = 0x0;    //defaulet 60M SPI
	int i = 0;
#ifdef _WIN32
    if (uhModule == 0) {
		uhModule = LoadLibrary("CH347DLLA64.DLL");
		if (uhModule) {
			CH347OpenDevice = (pCH347OpenDevice)GetProcAddress(
				uhModule, "CH347OpenDevice");
			CH347CloseDevice = (pCH347CloseDevice)GetProcAddress(
				uhModule, "CH347CloseDevice");
			CH347ReadData = (pCH347ReadData)GetProcAddress(
				uhModule, "CH347ReadData");
			CH347WriteData = (pCH347WriteData)GetProcAddress(
				uhModule, "CH347WriteData");
			CH347SetTimeout = (pCH347SetTimeout)GetProcAddress(
				uhModule, "CH347SetTimeout");
			if (CH347OpenDevice == NULL || CH347CloseDevice == NULL
			    || CH347SetTimeout == NULL || CH347ReadData == NULL
			    || CH347WriteData == NULL) {
				printf("ch347_spi_init error\n");
				return -1;
			}
		}
	}
	for (int i = 0;i < 16; i++){
		if(CH347OpenDevice(i) != -1){
			open_res = 0;
			ugIndex = i;
			break;
		}
	}
    if (open_res == -1){
        DevIsOpened = FALSE;
        printf("Couldn't open CH347 device.\n");
		return -1;
    }else {
        DevIsOpened = TRUE;
		printf("Open CH347 device success.\n");
    }
#elif defined(__linux__)
	int32_t ret = libusb_init(NULL);
	if (ret < 0) {
		printf("Could not initialize libusb!\n");
		return -1;
	}

	/* Enable information, warning, and error messages (only). */
#if LIBUSB_API_VERSION < 0x01000106
	libusb_set_debug(NULL, 3);
#else
	libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
#endif
	ch347_data->handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
	devHandle = ch347_data->handle;
	if (ch347_data->handle == NULL) {
		printf("Couldn't open device %04x:%04x.\n", vid, pid);
		free(ch347_data);
		return 1;
	}

	/* TODO: set based on mode */
	/* Mode 1 uses interface 2 for the SPI interface */
	int spi_interface = MODE_1_IFACE;

	ret = libusb_detach_kernel_driver(ch347_data->handle, spi_interface);
	if (ret != 0 && ret != LIBUSB_ERROR_NOT_FOUND)
		msg_pwarn("Cannot detach the existing USB driver. Claiming the interface may fail. %s\n",
			libusb_error_name(ret));

	ret = libusb_claim_interface(ch347_data->handle, spi_interface);
	if (ret != 0) {
		printf("Failed to claim interface 2: '%s'\n", libusb_error_name(ret));
		goto error_exit;
	}

	struct libusb_device *dev;
	if (!(dev = libusb_get_device(ch347_data->handle))) {
		printf("Failed to get device from device handle.\n");
		goto error_exit;
	}

	struct libusb_device_descriptor desc;
	ret = libusb_get_device_descriptor(dev, &desc);
	if (ret < 0) {
		printf("Failed to get device descriptor: '%s'\n", libusb_error_name(ret));
		goto error_exit;
	}

	msg_pdbg("Device revision is %d.%01d.%01d\n",
		(desc.bcdDevice >> 8) & 0x00FF,
		(desc.bcdDevice >> 4) & 0x000F,
		(desc.bcdDevice >> 0) & 0x000F);
#endif
	/* TODO: add programmer cfg for things like CS pin and divisor */
	if (config_stream(0) < 0)
		goto error_exit;
	return 0;
error_exit:
	ch347_spi_shutdown();
	return -1;
}
/* End of [ch347_spi.c] package */
