include ../Makefile.common

# System modules
USEMODULE += saul_default
USEMODULE += ztimer_msec
USEPKG += nanocbor

# Drivers
USEMODULE += bme280_i2c

# Project modules
USEMODULE += wsn
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../sys/wsn

include $(RIOTBASE)/Makefile.include
