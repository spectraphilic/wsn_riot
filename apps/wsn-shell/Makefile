include ../Makefile.common

#------ System modules --------------
USEMODULE += fmt
#USEMODULE += periph_rtc
USEMODULE += saul_default

# Shell
USEMODULE += i2c_scan
USEMODULE += shell
USEMODULE += shell_commands


#=====================================
#----- RIOT Native Drivers -----------
# BME280 (I2C)
USEMODULE += bme280_i2c

# SHT31 (I2C)
#USEMODULE += sht3x

# DS18B20 (1-WIRE)
#USEMODULE += ds18


#======================================
# Project drivers
#USEMODULE += lis3331ldh

# Project modules
USEMODULE += wsn

#======================================
#------ Custom Settings ---------------
# I2C address specification
CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77

include $(RIOTBASE)/Makefile.include
