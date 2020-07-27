include ../Makefile.common

# System modules
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += i2c_scan
#USEMODULE += periph_rtc

# RIOT Drivers
DRIVER ?= bme280_i2c
USEMODULE += fmt
USEMODULE += $(DRIVER)

USEMODULE += sht3x

# Project modules
USEMODULE += lis3331ldh
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../modules/lis3331ldh
#USEMODULE += progmem
#EXTERNAL_MODULE_DIRS += $(CURDIR)/../../modules/progmem

# I2C address specification
CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77

include $(RIOTBASE)/Makefile.include
