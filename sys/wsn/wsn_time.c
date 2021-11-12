// Riot
#include <log.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include <wsn.h>

#ifndef BASETIME
    #define BASETIME 0
#endif

static time_t basetime = BASETIME; // Seconds


time_t wsn_time_basetime(void)
{
    return basetime;
}

time_t wsn_time_get(void)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);
    return basetime + now / MS_PER_SEC;
}

int wsn_time_set(time_t time)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);

    // Not before 26 Oct 2021
    if (time < 1635199200) {
        LOG_WARNING("Unexpected value %lld", time);
        return -1;
    }

    // Set time
    basetime = time - now / MS_PER_SEC;
    LOG_INFO("Basetime set to %lld", basetime);
    return 0;
}
