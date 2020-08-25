#include <stdio.h>
#include "progmem.h"


int main(void)
{
    puts("app: test-progmem");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    printf("%S\n", PSTR("This string is stored in program memory"));
    printf("%s\n", "This is a regular string");

    return 0;
}
