// Riot
#include <log.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include <wsn.h>

#ifndef BASETIME
    #define BASETIME 0
#endif

static ztimer_now_t basetime = BASETIME; // Seconds


ztimer_now_t wsn_time_basetime(void)
{
    return basetime;
}

ztimer_now_t wsn_time_get(unsigned *ms)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);

    if (ms != NULL) {
        *ms = now % MS_PER_SEC;
    }

    return basetime + now / MS_PER_SEC;
}

int wsn_time_set(ztimer_now_t time)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);

    // Not before 26 Oct 2021
    if (time < 1635199200L) {
        LOG_WARNING("Unexpected value %ld", time);
        return -1;
    }

    // Set time
    basetime = time - now / MS_PER_SEC;
    LOG_INFO("Basetime set to %ld", basetime);
    return 0;
}
