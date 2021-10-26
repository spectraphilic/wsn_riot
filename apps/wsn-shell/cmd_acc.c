#ifdef MODULE_LIS3331LDH

// RIOT
#include "periph/i2c.h"

// WSN
#include "lis3331ldh.h"


int cmd_acc(int argc, char **argv)
{
    const i2c_t dev = I2C_DEV(0);
    int error = 0;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }
    assert(argv); // Avoids warning

    // Acquire
    error = i2c_acquire(dev);
    if (error)
    {
        printf("i2c_acquire error=%d\n", error);
        return error;
    }

    // Initialize
    error = lis3331ldh_init(dev);
    if (error) {
        printf("ACC check error\n");
        goto exit;
    }

    // Read
    int16_t x, y, z;
    error = lis3331ldh_read_xyz(dev, &x, &y, &z);
    if (error) {
        printf("ACC Error\n");
        goto exit;
    }

    printf("ACC x=%d y=%d z=%d\n", x, y, z);

exit:
    // Release
    i2c_release(dev);
    return error;
}

#endif
