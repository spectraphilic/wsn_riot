// Standard
#include <stdio.h>

// Riot
#include <log.h>

// Project
#include "progmem.h"


int main(void)
{
    LOG_INFO("app=test-progmem board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);

    printf("%S\n", PSTR("This string is stored in program memory"));
    printf("%s\n", "This is a regular string");

    return 0;
}
