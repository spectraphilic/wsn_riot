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

CFLAGS += -DNODE_ID=\"$(NODE_ID)\"

include $(RIOTBASE)/Makefile.include
