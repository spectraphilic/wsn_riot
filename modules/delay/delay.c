#include "xtimer.h"

unsigned long millis(void)
{
    return xtimer_now_usec64() / US_PER_MS;
}

void delay(unsigned long ms)
{
#ifdef WASPMOTE_PINMAP_H
    unsigned long start = millis();
    while (millis() - start < ms)
    {
        if(millis() < start)
        {
            start = millis();
        }
    }
#else
    xtimer_usleep(ms * US_PER_MS);
#endif
}
