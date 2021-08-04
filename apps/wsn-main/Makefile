include ../Makefile.common

# System modules
USEMODULE += od
USEMODULE += saul_default
USEMODULE += ztimer_msec
USEPKG += nanocbor

# Drivers
#USEMODULE += bme280_i2c

# Project modules
USEMODULE += wsn

CFLAGS += -DNODE_ID=\"$(NODE_ID)\"

include $(RIOTBASE)/Makefile.include
