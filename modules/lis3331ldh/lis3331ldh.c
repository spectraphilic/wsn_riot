#include "debug.h"
#include "periph/i2c.h"

#include "delay.h"


const uint16_t ADDRESS = 0x19;
const uint16_t WHO_AM_I = 0x0F;
const uint16_t CTRL_REG1 = 0x20;
const uint16_t CTRL_REG4 = 0x23;
const uint8_t flags = 0;


int lis3331ldh_init(i2c_t dev)
{
    int error;
    uint8_t data;

    // Power on
    error = i2c_write_reg(dev, ADDRESS, CTRL_REG1, 39, flags);
    if (error) {
        DEBUG("[lis3331ldh] error powering on: %d\n", error);
        return -1;
    }
    //delay(21);

    // Configure
    error = i2c_write_reg(dev, ADDRESS, CTRL_REG4, 0, flags); // 2G
    if (error) {
        DEBUG("[lis3331ldh] error configuring: %d\n", error);
        return -1;
    }

    // Check status
    error = i2c_read_reg(dev, ADDRESS, WHO_AM_I, &data, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
    } else if (data != 0x32) {
        printf("acc unexpected data=%u\n", data);
        return -1;
    }

    return 0;
}
