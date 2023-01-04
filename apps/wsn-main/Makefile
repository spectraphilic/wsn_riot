include ../Makefile.common

# System modules
USEMODULE += od
USEMODULE += phydat
USEMODULE += rtc_utils
USEMODULE += ztimer_msec
USEPKG += nanocbor

# Project modules
USEMODULE += gps
USEMODULE += wsn

# Optional modules
USEMODULE += ps

ifdef BASETIME
    CFLAGS += -DBASETIME=$(BASETIME)
endif

ifdef NODE_ID
    CFLAGS += -DNODE_ID=\"$(NODE_ID)\"
else
    $(error NODE_ID is not set)
endif

include $(RIOTBASE)/Makefile.include
