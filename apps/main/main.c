// Riot
#include <log.h>
#include <ztimer.h>

// Riot
#include "wsn.h"


#define SLEEP 5L * 1000000L // 5 seconds


static void callback(void *arg)
{
   LOG_INFO(arg);
}


int main(void)
{
    // Boot
    LOG_INFO("app=main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for 5s in every loop.");
    wsn_boot();

    ztimer_t timeout = { .callback=callback, .arg="Hello ztimer!\n" };
    ztimer_set(ZTIMER_USEC, &timeout, 2000000);

    // Main loop
    for (unsigned int i=0; ; i++) {
        LOG_INFO("Loop=%u\n", i);
        ztimer_sleep(ZTIMER_USEC, SLEEP);
    }

    return 0;
}
