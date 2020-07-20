#include "debug.h"
#include "periph/i2c.h"

#include "delay.h"


const uint16_t ADDRESS = 0x19;
const uint16_t WHO_AM_I = 0x0F;
const uint16_t CTRL_REG1 = 0x20;
const uint16_t CTRL_REG4 = 0x23;
const uint8_t flags = 0;

const uint8_t OUT_X_L = 0x28;
const uint8_t OUT_X_H = 0x29;
const uint8_t OUT_Y_L = 0x2A;
const uint8_t OUT_Y_H = 0x2B;
const uint8_t OUT_Z_L = 0x2C;
const uint8_t OUT_Z_H = 0x2D;


int lis3331ldh_init(i2c_t dev)
{
    int error;
    uint8_t data;

    // Power on
    error = i2c_write_reg(dev, ADDRESS, CTRL_REG1, 39, flags);
    if (error) {
        DEBUG("[lis3331ldh] error powering on: %d\n", error);
        return error;
    }
    //delay(21);

    // Configure
    error = i2c_write_reg(dev, ADDRESS, CTRL_REG4, 0, flags); // 2G
    if (error) {
        DEBUG("[lis3331ldh] error configuring: %d\n", error);
        return error;
    }

    // Check status
    error = i2c_read_reg(dev, ADDRESS, WHO_AM_I, &data, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    } else if (data != 0x32) {
        printf("acc unexpected data=%u\n", data);
        return -1;
    }

    return 0;
}


int lis3331ldh_read_xyz(i2c_t dev, int16_t *x, int16_t *y, int16_t *z)
{
    const int16_t fs = 2; // FS_2G
    int error;
    uint8_t h, l;
    int32_t aux;

    // X axis
    error = i2c_read_reg(dev, ADDRESS, OUT_X_H, &h, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    error = i2c_read_reg(dev, ADDRESS, OUT_X_L, &l, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    aux = ((int8_t)h*256) + l;
    *x = fs * 1000 * (aux)/32768;

    // X axis
    error = i2c_read_reg(dev, ADDRESS, OUT_Y_H, &h, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    error = i2c_read_reg(dev, ADDRESS, OUT_Y_L, &l, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    aux = ((int8_t)h*256) + l;
    *y = fs * 1000 * (aux)/32768;

    // Z axis
    error = i2c_read_reg(dev, ADDRESS, OUT_Z_H, &h, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    error = i2c_read_reg(dev, ADDRESS, OUT_Z_L, &l, flags);
    if (error) {
        DEBUG("[lis3331ldh] error reading register: %d\n", error);
        return error;
    }

    aux = ((int8_t)h*256) + l;
    *z = fs * 1000 * (aux)/32768;

    // Ok
    return 0;
}
