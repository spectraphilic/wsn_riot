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

int wsn_time_load(void)
{
    if (wsn_rtc_init() == 0) {
        struct tm time;
        if (wsn_rtc_time_get(&time) == 0) {
            basetime = (ztimer_now_t)mktime(&time); // time_t to ztimer_now_t
            if (basetime > 0) {
                return 0;
            }
        }
    }

    return -1;
}

static int wsn_time_save(void)
{
    if (wsn_rtc_init() == 0) {
        time_t timex = (time_t)wsn_time_get(NULL);
        struct tm *time = gmtime(&timex);

        if (wsn_rtc_time_set(time) == 0) {
            return 0;
        }
    }

    return -1;
}

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

    // Set system time
    basetime = time - now / MS_PER_SEC;
    LOG_INFO("Basetime set to %ld", basetime);

    // Save time to RTC
    return wsn_time_save();
}
