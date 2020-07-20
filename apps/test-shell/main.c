#include <assert.h>
#include <stdio.h>

// RIOT
#include "periph/i2c.h"
#include "shell.h"

// WSN
#include "lis3331ldh.h"


static int acc(int argc, char **argv) {
    const i2c_t dev = I2C_DEV(0);
    int error;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return 1;
    }
    assert(argv); // Avoids warning

    // Acquire
    error = i2c_acquire(dev);
    if (error)
    {
        printf("i2c_acquire error=%d\n", error);
        return 1;
    }

    // Initialize
    error = lis3331ldh_init(dev);
    if (error) {
        printf("ACC Error\n");
    } else {
        printf("ACC OK\n");
    }

    // Release
    i2c_release(dev);

    return error;
}


/*
static int echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    return 0;
}
*/

const shell_command_t shell_commands[] = {
    {"acc", "accelerometer", acc},
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
