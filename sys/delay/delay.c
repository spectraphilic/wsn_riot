#include "ztimer.h"

unsigned long millis(void)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);
    return now;
}

void delay(unsigned long ms)
{
    ztimer_sleep(ZTIMER_MSEC, ms);
}
