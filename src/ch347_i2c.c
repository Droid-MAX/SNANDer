//
// ch347eeprom programmer version 0.1 (Beta)
//
//  Programming tool for the 24Cxx serial EEPROMs using the Winchiphead ch347 IC
//
// (c) December 2011 asbokid <ballymunboy@gmail.com>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <libusb-1.0/libusb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ch347_i2c.h"
#define dprintf(args...)
// #define dprintf(args...) do { if (1) printf(args); } while(0)
#define true 1
#define false 0
extern struct libusb_device_handle *handle;
unsigned char *readbuf;
uint32_t getnextpkt; // set by the callback function
uint32_t syncackpkt; // synch / ack flag used by BULK OUT cb function
uint32_t byteoffset;

// callback functions for async USB transfers
static void cbBulkIn(struct libusb_transfer *transfer);
static void cbBulkOut(struct libusb_transfer *transfer);
#ifdef _WIN32
extern pCH347ReadData CH347ReadData;
extern pCH347WriteData CH347WriteData;
extern ULONG ugIndex;
extern struct libusb_device_handle *devHandle;
#endif

struct xxx {
  uint8_t ibuf[512];
  uint8_t obuf[512];
} i2c_dev;

void ch347ReadCmdMarshall(uint8_t *buffer, uint32_t addr, struct EEPROM *eeprom_info)
{
	uint8_t *ptr = buffer;
	uint8_t msb_addr;
	uint32_t size_kb;
	
	*ptr++ = mch347_CMD_I2C_STREAM; // 0
	*ptr++ = mch347_CMD_I2C_STM_STA; // 1
	// Write address
	*ptr++ = mch347_CMD_I2C_STM_OUT | ((*eeprom_info).addr_size + 1); // 2: I2C bus adddress + EEPROM address
	if ((*eeprom_info).addr_size >= 2) {
		// 24C32 and more
		msb_addr = addr >> 16 & (*eeprom_info).i2c_addr_mask;
		*ptr++ = (EEPROM_I2C_BUS_ADDRESS | msb_addr) << 1; // 3
		*ptr++ = (addr >> 8 & 0xFF); // 4
		*ptr++ = (addr >> 0 & 0xFF); // 5
	} else {
		// 24C16 and less
		msb_addr = addr >> 8 & (*eeprom_info).i2c_addr_mask;
		*ptr++ = (EEPROM_I2C_BUS_ADDRESS | msb_addr) << 1; // 3
		*ptr++ = (addr >> 0 & 0xFF); // 4
	}
	// Read
	*ptr++ = mch347_CMD_I2C_STM_STA; // 6/5
	*ptr++ = mch347_CMD_I2C_STM_OUT | 1; // 7/6
	*ptr++ = ((EEPROM_I2C_BUS_ADDRESS | msb_addr) << 1) | 1; // 8/7: Read command

	// Configuration?
	*ptr++ = 0xE0; // 9/8
	*ptr++ = 0x00; // 10/9
	if ((*eeprom_info).addr_size < 2)
		*ptr++ = 0x10; // x/10
	memcpy(ptr, "\x00\x06\x04\x00\x00\x00\x00\x00\x00", 9);
	ptr += 9; // 10
	size_kb = (*eeprom_info).size/1024;
	*ptr++ = size_kb & 0xFF; // 19
	*ptr++ = (size_kb >> 8) & 0xFF; // 20
	memcpy(ptr, "\x00\x00\x11\x4d\x40\x77\xcd\xab\xba\xdc", 10);
	ptr += 10;

	// Frame 2
	*ptr++ = mch347_CMD_I2C_STREAM;
	memcpy(ptr, "\xe0\x00\x00\xc4\xf1\x12\x00\x11\x4d\x40\x77\xf0\xf1\x12\x00" \
		    "\xd9\x8b\x41\x7e\x00\xe0\xfd\x7f\xf0\xf1\x12\x00\x5a\x88\x41\x7e", 31);
	ptr += 31;

	// Frame 3
	*ptr++ = mch347_CMD_I2C_STREAM;
	memcpy(ptr, "\xe0\x00\x00\x2a\x88\x41\x7e\x06\x04\x00\x00\x11\x4d\x40\x77" \
		    "\xe8\xf3\x12\x00\x14\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 31);
	ptr += 31;

	// Finalize
	*ptr++ = mch347_CMD_I2C_STREAM; // 0xAA
	*ptr++ = 0xDF; // ???
	*ptr++ = mch347_CMD_I2C_STM_IN; // 0xC0
	*ptr++ = mch347_CMD_I2C_STM_STO; // 0x75
	*ptr++ = mch347_CMD_I2C_STM_END; // 0x00

	assert(ptr - buffer == CH347_EEPROM_READ_CMD_SZ);
}

int ch347_i2c_write(struct i2c_msg *msg) {
    unsigned left = msg->len;
    uint8_t *ptr = msg->buf;
    boolean first = true;
    do {
        uint8_t *outptr = i2c_dev.obuf;
        *outptr++ = mch347_CMD_I2C_STREAM;
        unsigned wlen = left;
        if (wlen > 62) { // wlen has only 6-bit field in protocol
            wlen = 62;
        }
        if (first) { // Start packet
            *outptr++ = mch347_CMD_I2C_STM_STA;
            *outptr++ = mch347_CMD_I2C_STM_OUT | (wlen + 1);
            *outptr++ = msg->addr << 1;
        }
        memcpy(outptr, ptr, wlen);
        outptr += wlen;
        ptr += wlen;
        left -= wlen;

        if (left == 0) {  // Stop packet
            *outptr++ = mch347_CMD_I2C_STM_STO;
        }
        *outptr++ = mch347_CMD_I2C_STM_END;
        first = false;
        int ret = 0;
        unsigned long actuallen = 0;
#ifdef _WIN32
        actuallen = outptr - i2c_dev.obuf;
        if (!CH347WriteData(ugIndex, i2c_dev.obuf, &actuallen)){
            fprintf(stderr, "USB write error\r\n");
            return -1;
        }
        actuallen = 512;
        if (!CH347ReadData(ugIndex, i2c_dev.ibuf, &actuallen)){
            fprintf(stderr, "USB read error\r\n");
            return -1;
        }
#elif defined(__linux__)
        ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, outptr - i2c_dev.obuf, &actuallen, DEFAULT_TIMEOUT);

        if (ret < 0) {
            fprintf(stderr, "Failed to write to I2C: '%s'\r\n", strerror(-ret));
            return -1;
        }
        ret = libusb_bulk_transfer(devHandle, BULK_READ_ENDPOINT, i2c_dev.ibuf, 512, &actuallen, DEFAULT_TIMEOUT);

        if (ret < 0) {
            fprintf(stderr, "Failed to write to I2C: '%s'\r\n", strerror(-ret));
            return -1;
        }
#endif  
        if (wlen + 1 != actuallen) {
            fprintf(stderr, "failed to get ACKs %d of %d\r\n", actuallen, wlen + 1);
            return -1;
        }
        for (unsigned i = 0; i < actuallen; ++i) {
            if (i2c_dev.ibuf[i] != 0x01) {
                fprintf(stderr, "received NACK at %d\r\n", i);
                return -1;
            }
        }
    } while (left);
    return 0;
}

int ch347_i2c_read(struct i2c_msg *msg)
{
    unsigned byteoffset = 0;
    while (msg->len - byteoffset > 0) {
        unsigned bytestoread = msg->len - byteoffset;
        if (bytestoread > 63) // reserve first byte for status
            bytestoread = 63;
        uint8_t *ptr = i2c_dev.obuf;
        *ptr++ = mch347_CMD_I2C_STREAM;
        *ptr++ = mch347_CMD_I2C_STM_STA;
        *ptr++ = mch347_CMD_I2C_STM_OUT|1;
        *ptr++ = (msg->addr << 1) | 1;
        if (bytestoread > 1)
            *ptr++ = mch347_CMD_I2C_STM_IN | (bytestoread - 1);
        *ptr++ = mch347_CMD_I2C_STM_IN;
        *ptr++ = mch347_CMD_I2C_STM_STO;
        *ptr++ = mch347_CMD_I2C_STM_END;
        unsigned long actuallen = 0;
#ifdef _WIN32
        actuallen = ptr - i2c_dev.obuf;
        if (!CH347WriteData(ugIndex, i2c_dev.obuf, &actuallen)){
            fprintf(stderr, "USB write error\r\n");
            return -1;
        }
        actuallen = 512;
        if (!CH347ReadData(ugIndex, i2c_dev.ibuf, &actuallen)){
            fprintf(stderr, "USB read error\r\n");
            return -1;
        }
#elif defined(__linux__)
        int ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, ptr - i2c_dev.obuf, &actuallen, DEFAULT_TIMEOUT);
        if (ret < 0) {
            fprintf(stderr, "USB write error : %s\r\n", strerror(-ret));
            return ret;
        }
        ret = libusb_bulk_transfer(devHandle, BULK_READ_ENDPOINT, i2c_dev.ibuf, 512, &actuallen, DEFAULT_TIMEOUT);
        if (ret < 0) {
            fprintf(stderr, "USB read error : %s\r\n", strerror(-ret));
            return ret;
        }
#endif   
        if (actuallen != bytestoread + 1) {
            fprintf(stderr, "actuallen(%d) != bytestoread(%d)\b", actuallen, bytestoread);
            return -1;
        }
        if (i2c_dev.ibuf[0] != 0x01) {
            fprintf(stderr, "received NACK");
            return -1;
        }
        memcpy(&msg->buf[byteoffset], &i2c_dev.ibuf[1], bytestoread);
        byteoffset += bytestoread;
    }
    return 0;
}

int ch347_i2c_xfer(struct i2c_msg *msg, unsigned num) {
    for (unsigned i = 0; i < num; ++i) {
        if (msg[i].flags & I2C_M_RD) {
            int ret = ch347_i2c_read(&msg[i]);
            if (ret) return ret;
        } else {
            int ret = ch347_i2c_write(&msg[i]);
            if (ret) return ret;
        }
    }
    return 0;
}

// Callback function for async bulk in comms
void cbBulkIn(struct libusb_transfer *transfer)
{
	int i;

	switch (transfer->status) {
		case LIBUSB_TRANSFER_COMPLETED:
			// display the contents of the BULK IN data buffer
			dprintf("\ncbBulkIn(): status %d - Read %d bytes\n",transfer->status,transfer->actual_length);

			for (i = 0; i < transfer->actual_length; i++) {
				if(!(i % 16))
					dprintf("\n   ");
				dprintf("%02x ", transfer->buffer[i]);
			}
			dprintf("\n");
			// copy read data to our EEPROM buffer
			memcpy(readbuf + byteoffset, transfer->buffer, transfer->actual_length);
			getnextpkt = 1;
			break;
		default:
			printf("\ncbBulkIn: error : %d\n", transfer->status);
			getnextpkt = -1;
	}
	return;
}

// Callback function for async bulk out comms
void cbBulkOut(struct libusb_transfer *transfer)
{
	syncackpkt = 1;
	dprintf("\ncbBulkOut(): Sync/Ack received: status %d\n", transfer->status);
	return;
}

// --------------------------------------------------------------------------
// ch347readEEPROM()
//      read n bytes from device (in packets of 32 bytes)
int ch347readEEPROM(uint8_t *buffer, uint32_t bytes, struct EEPROM *eeprom_info)
{
    struct i2c_msg msg[2];

    uint8_t out[2] = {0};
    msg[0].len = eeprom_info->addr_size;
    msg[0].flags = 0;
    msg[0].addr = EEPROM_I2C_BUS_ADDRESS;
    msg[0].buf = out;

    msg[1].flags = I2C_M_RD;
    msg[1].buf = buffer;
    msg[1].addr = EEPROM_I2C_BUS_ADDRESS;
    msg[1].len = bytes;

    return ch347_i2c_xfer(msg, 2);
}

int ch347delay_ms(unsigned ms) {
    i2c_dev.obuf[0] = mch347_CMD_I2C_STREAM;
    i2c_dev.obuf[1] = mch347_CMD_I2C_STM_MS | (ms & 0xf);        // Wait up to 15ms
    i2c_dev.obuf[2] = mch347_CMD_I2C_STM_END;
    unsigned long actuallen = 0;
#ifdef _WIN32
    actuallen = 3;
    if (!CH347WriteData(ugIndex, i2c_dev.obuf, &actuallen) || actuallen != 3){
        fprintf(stderr, "USB write error.\r\n");
        return -1;
    }
#elif defined(__linux__)
    libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, 3, &actuallen, DEFAULT_TIMEOUT);
#endif
    return 0;
}

// --------------------------------------------------------------------------
// ch347writeEEPROM()
//      write n bytes to 24c32/24c64 device (in packets of 32 bytes)
int32_t ch347writeEEPROM(uint8_t *buffer, uint32_t bytesum, struct EEPROM *eeprom_info)
{
	uint8_t msgbuffer[256 + 2]; // max EEPROM page size is 256 in 2M part, and 2 bytes for address
    int index = 0;
    struct i2c_msg msg;
    msg.addr = EEPROM_I2C_BUS_ADDRESS;
    msg.buf = msgbuffer;
    msg.flags = 0;
    unsigned offset = 0;
    while (offset < bytesum) {
        uint8_t *outptr = msgbuffer;
        unsigned wlen = eeprom_info->page_size;
        if (eeprom_info->addr_size > 1) {
            *outptr++ = offset >> 8;
        }
        *outptr++ = offset;
        if (bytesum - offset < wlen) {
            wlen = bytesum - offset;
        }
        memcpy(outptr, buffer + offset, wlen);
        outptr += wlen;
        msg.len = outptr - msgbuffer;
        int ret = 0;
        ret = ch347_i2c_xfer(&msg, 1);
        if (ret < 0) {
            fprintf(stderr, "Failed to write to EEPROM: '%s'\r\n", strerror(-ret));
            return -1;
        }
        offset += wlen;

        ret = ch347delay_ms(10);
        if (ret < 0) {
            fprintf(stderr, "Failed to set timeout: '%s'\r\n", strerror(-ret));
            return -1;
        }
        fprintf(stdout, "Written %d%% [%d] of [%d] bytes%s", 100 * offset / bytesum, offset, bytesum, ((index++)%2)?"\t":"\r\n");
    }
    return 0;
}

// --------------------------------------------------------------------------
// parseEEPsize()
//   passed an EEPROM name (case-sensitive), returns its byte size
int32_t parseEEPsize(char *eepromname, struct EEPROM *eeprom)
{
	int i;

	for (i = 0; eepromlist[i].size; i++) {
		if (strstr(eepromlist[i].name, eepromname)) {
			memcpy(eeprom, &(eepromlist[i]), sizeof(struct EEPROM));
			return (eepromlist[i].size);
		}
	}

	return -1;
}
