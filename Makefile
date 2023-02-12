include $(TOPDIR)/rules.mk

PKG_NAME:=snander
PKG_VERSION:=1.7.8
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=libpthread libusb-1.0
PKG_MAINTAINER:=McMCC <mcmcc@mail.ru>
PKG_LICENSE:=GPLv2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Serial Nor/nAND/Eeprom programmeR (based on CH341A)
	MAINTAINER:=McMCC <mcmcc@mail.ru>
	URL:=https://github.com/McMCCRU/SNANDer
	DEPENDS:=+libpthread +libusb-1.0
	TARGET_CFLAGS:=-std=gnu99 -Wall -O2 -D_FILE_OFFSET_BITS=64
	TARGET_LDFLAGS:=-ldl -lpthread
endef

define Package/$(PKG_NAME)/description
	Serial Nor/nAND/Eeprom programmeR (based on CH341A)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -r src/* $(PKG_BUILD_DIR)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snander $(1)/usr/bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
