#include <stdio.h>

int main(void)
{
    puts("RIOT Test program");
    printf("Board: %s\n", RIOT_BOARD);
    printf("MCU  : %s\n", RIOT_MCU);

    return 0;
}
