#include <stdio.h>
#include "hello.h"


int main(void)
{
    puts("RIOT Test program");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);
    hello("UiO");

    return 0;
}
