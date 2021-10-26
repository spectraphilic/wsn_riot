// Riot
#include <log.h>
#include <timex.h>
#include <ztimer.h>


#ifdef MODULE_ZTIMER_MSEC
    #define ZTIMER ZTIMER_MSEC
    #define TICKS_PER_SEC MS_PER_SEC
#else
    #define ZTIMER ZTIMER_USEC
    #define TICKS_PER_SEC US_PER_SEC
#endif

#ifndef BASETIME
    #define BASETIME 0
#endif


static unsigned int basetime = BASETIME; // Seconds


time_t wsn_time_basetime(void)
{
    return basetime;
}

time_t wsn_time_get(void)
{
    ztimer_now_t now = ztimer_now(ZTIMER);
    return basetime + now / TICKS_PER_SEC;
}

int wsn_time_set(time_t time)
{
    ztimer_now_t now = ztimer_now(ZTIMER);

    // Not before 26 Oct 2021
    if (time < 1635199200) {
        LOG_WARNING("Unexpected value %lu\n", time);
        return -1;
    }

    // Set time
    basetime = time - now / TICKS_PER_SEC;
    LOG_INFO("Basetime set to %lu\n", basetime);
    return 0;
}
