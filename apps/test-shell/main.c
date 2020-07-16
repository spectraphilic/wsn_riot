#include <stdio.h>

#include "shell.h"


static int echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    return 0;
}

const shell_command_t shell_commands[] = {
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
