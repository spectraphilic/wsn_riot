PKG_NAME    = arduino_sparkfun_bme280
PKG_URL     = https://github.com/sparkfun/SparkFun_BME280_Arduino_Library/archive/v2.0.8
PKG_VERSION = v2.0.8
PKG_EXT     = zip
PKG_LICENSE = MIT

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean patch distclean

prepare: $(PKG_SOURCE_DIR)/

all:
	$(MAKE) -C $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)
#	$(MAKE) -C $(PKG_BUILDDIR) -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

patch: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/Makefile
	# Dependancy might be changed accordingly though we think the Makefile
	# will be the first thing you want to change
	#
	# Here might not happen anything besides dependancy checks

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/Makefile: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/
	# Here you apply your patch.
	cd $< && patch ../patch.txt

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	# Here you unpack the file.
	# This example assumes the common pattern that the archive contains its data in a subfolder with the same name as itself.
	$(Q)$(UNZIP_HERE) $<

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT):
	# Get PKG_VERSION of package from PKG_URL
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)

clean::
	# Reset package to checkout state.
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION) && \
		$(MAKE) $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/Makefile

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION) $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
