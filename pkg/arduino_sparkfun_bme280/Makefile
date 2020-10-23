PKG_NAME    = arduino_sparkfun_bme280
PKG_VERSION = 2.0.8
PKG_EXT     = zip
PKG_LICENSE = MIT

PKG_SRC_NAME = SparkFun_BME280_Arduino_Library
PKG_URL      = https://github.com/sparkfun/$(PKG_SRC_NAME)/archive/v$(PKG_VERSION).zip
PKG_ZIPFILE  = $(PKGDIRBASE)/$(PKG_SRC_NAME)-v$(PKG_VERSION).$(PKG_EXT)
PKG_SOURCE_DIR = $(PKGDIRBASE)/$(PKG_SRC_NAME)-$(PKG_VERSION)
PKG_TARGET_DIR = $(PKGDIRBASE)/$(PKG_NAME)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean patch distclean

CFLAGS += -DARDUINO=200

prepare: $(PKG_TARGET_DIR)

all: patch
	"$(MAKE)" -C $(PKG_TARGET_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

patch:
	# PATCH
	cd $(PKG_TARGET_DIR)/src && patch --forward < $(CURDIR)/patches/math.patch || true

$(PKG_TARGET_DIR): $(PKG_ZIPFILE)
	# UNPACK
	$(Q)$(UNZIP_HERE) -D -d $(PKGDIRBASE) $<
	mv $(PKG_SOURCE_DIR) $(PKG_TARGET_DIR)

$(PKG_ZIPFILE):
	# DOWNLOAD
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)

clean::
	# Reset package to checkout state.
	rm -rf $(PKG_TARGET_DIR) $(PKG_SOURCE_DIR) && $(MAKE) $(PKG_TARGET_DIR)

distclean::
	rm -rf $(PKG_TARGET_DIR) $(PKG_SOURCE_DIR) $(PKG_ZIPFILE)
