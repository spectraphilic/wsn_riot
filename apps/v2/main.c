#include <stdio.h>
#include "shell.h"
#include "xtimer.h"


#define SLEEP 3


/*
 * Utilities. Eventually these will be moved to modules.
 */
void info(const char* msg) {
    uint32_t now = xtimer_now_usec(); // miliseconds
    printf("%15.6f %s\n", ((float)now) / 1000000, msg);
}


/*
 * Shell custom commands
 */

static int echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    return 0;
}

static int exit_shell(int argc, char **argv) {
    /* Suppress compiler errors */
    (void)argc;
    (void)argv;

    printf("bye\n");
    return 1;
}

const shell_command_t shell_commands[] = {
    {"echo", "prints given arguments", echo},
    {"exit", "exit the shell (doesn't work, to exit type ^D ENTER)", exit_shell},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Boot
    puts("*** UiO WSN v2 ***");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    // Shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
//  shell_run_once(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    // Main loop
    unsigned int i = 0;
    while (1) {
      puts("Sleep...");
      xtimer_sleep(SLEEP);
      puts("Awake!");
      snprintf(buffer, sizeof(buffer), "Loop=%u", i);
      info(buffer);
      i += 1;
    }

    return 0;
}
