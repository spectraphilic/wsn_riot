include ../Makefile.common

#------ System modules --------------
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += i2c_scan
USEMODULE += fmt
#USEMODULE += periph_rtc


#=====================================
#----- RIOT Native Drivers -----------
# BME280 (I2C)
DRIVER ?= bme280_i2c
USEMODULE += $(DRIVER)

# SHT31 (I2C)
USEMODULE += sht3x

# DS18B20 (1-WIRE)
USEMODULE += ds18


#======================================
#------ Project modules ---------------
USEMODULE += lis3331ldh
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../modules/lis3331ldh
#USEMODULE += progmem
#EXTERNAL_MODULE_DIRS += $(CURDIR)/../../modules/progmem


#======================================
#------ Custom Settings ---------------
# I2C address specification
CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77

include $(RIOTBASE)/Makefile.include
