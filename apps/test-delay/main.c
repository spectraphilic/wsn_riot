#include <stdio.h>
#include "delay.h"


int main(void)
{
    puts("app: test-delay");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    const unsigned long ms = 5000;
    while (1) {
        printf("delay(%lu)\n", ms);
        delay(ms);
    }

    return 0;
}
