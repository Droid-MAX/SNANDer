TARGET     = snander
PKG        = $(TARGET)

CC        ?= gcc
STRIP     ?= strip
INSTALL   ?= install
WINDRES   ?= windres
PREFIX    ?= /usr
BINDIR    ?= $(PREFIX)/bin
CFLAGS    ?= -std=gnu99 -Wall -O2
LDFLAGS   ?= -pthread
RES       ?= res.bin

EEPROM_SUPPORT = yes

SRCS = flashcmd_api.c spi_controller.c spi_nand_flash.c spi_nor_flash.c ch341a_spi.c timer.c main.c

ifdef LIBS_BASE
CFLAGS += -I$(LIBS_BASE)/include
LDFLAGS += -L$(LIBS_BASE)/lib -Wl,-rpath -Wl,$(LIBS_BASE)/lib
endif

ifeq ($(CONFIG_STATIC), yes)
LDFLAGS += -static
endif

ifeq ($(EEPROM_SUPPORT), yes)
CFLAGS += -DEEPROM_SUPPORT
SRCS += ch341a_i2c.c i2c_eeprom.c bitbang_microwire.c mw_eeprom.c ch341a_gpio.c
endif

ifeq ($(TARGET_OS), MinGW)
EXEC_SUFFIX := .exe
SRCS += $(RES)
CFLAGS += -posix
CFLAGS += -Dffs=__builtin_ffs
CFLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif

$(PKG): $(TARGET)$(EXEC_SUFFIX)

$(TARGET)$(EXEC_SUFFIX): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -lusb-1.0 -o $@

$(RES):
	$(WINDRES) -i snander.rc -o $@

clean: 
	rm -f $(TARGET)$(EXEC_SUFFIX) $(RES)

strip: $(TARGET)$(EXEC_SUFFIX)
	$(STRIP) $(TARGET)$(EXEC_SUFFIX)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)/etc/udev/rules.d
	$(INSTALL) -m 0755 $(TARGET) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0664 40-persistent-ch341a.rules $(DESTDIR)/etc/udev/rules.d/40-persistent-ch341a.rules

install-udev-rule:
	cp 40-persistent-ch341a.rules /etc/udev/rules.d/
	udevadm control --reload-rules
.PHONY: clean install-udev-rule

debian/changelog:
	dch --create -v 1.7-1 --package $(PKG)

deb:
	dpkg-buildpackage -b -us -uc
.PHONY: install debian/changelog deb
