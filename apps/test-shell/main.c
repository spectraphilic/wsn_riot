#include <assert.h>
#include <stdio.h>

#include "periph/i2c.h"
#include "shell.h"
#include "delay.h"


static int acc(int argc, char **argv) {
    const i2c_t dev = 0;
    const uint16_t addr = 0x19;
    uint8_t data;
    const uint8_t flags = I2C_REG16; //I2C_ADDR10;
    int error;

    // Registers
    const uint16_t WHO_AM_I = 0x0F;
    const uint16_t CTRL_REG1 = 0x20;
    const uint16_t CTRL_REG4 = 0x23;

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return 1;
    }
    assert(argv); // Avoids warning

    // Acquire
    //i2c_init(dev);
    error = i2c_acquire(dev);
    if (error)
    {
        printf("i2c_acquire error=%d\n", error);
        return 1;
    }

    // Switch on
    error = i2c_write_reg(dev, addr, CTRL_REG1, 39, flags); // ON
    if (error) {
        printf("i2c_write_reg error=%d\n", error);
        goto exit;
    }
    delay(21);

    error = i2c_write_reg(dev, addr, CTRL_REG4, 0, flags); // 2G
    if (error) {
        printf("i2c_write_reg error=%d\n", error);
        goto exit;
    }

    // Check status
    error = i2c_read_reg(dev, addr, WHO_AM_I, &data, flags);
    if (error) {
        printf("i2c_read_reg error=%d\n", error);
    } else if (data == 0x32) {
        printf("acc success\n");
    } else {
        printf("acc unexpected data=%u\n", data);
    }

exit:
    // Release
    i2c_release(dev);

    return error;
}


static int echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    return 0;
}

const shell_command_t shell_commands[] = {
    {"acc", "accelerometer", acc},
    {"echo", "prints given arguments", echo},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Print some info
    puts("app: test-shell");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
