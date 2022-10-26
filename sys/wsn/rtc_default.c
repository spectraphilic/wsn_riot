#if !defined(MODULE_DS1307) && !defined(MODULE_DS3231)

#include <log.h>

int wsn_rtc_init(void)
{
    LOG_WARNING("Missing RTC module");
    return -1;
}

int wsn_rtc_temp(int16_t *temp) {
    (void)temp;
    LOG_WARNING("Missing RTC module");
    return -1;
}

int wsn_rtc_time_get(struct tm *time)
{
    (void)time;
    LOG_WARNING("Missing RTC module");
    return -1;
}

int wsn_rtc_time_set(struct tm *time)
{
    (void)time;
    LOG_WARNING("Missing RTC module");
    return -1;
}

#endif
