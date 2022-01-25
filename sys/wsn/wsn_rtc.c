// Riot
#ifdef MODULE_DS3231
#include <ds3231.h>
#include <ds3231_params.h>
#endif

// Project
#include <log.h>


#ifdef MODULE_DS3231
static ds3231_t _dev;
#endif

int wsn_rtc_init(void)
{
#ifdef MODULE_DS3231
    /* initialize the device */
    ds3231_params_t params= ds3231_params[0];
    params.opt     = DS3231_OPT_BAT_ENABLE;
    params.opt    |= DS3231_OPT_INTER_ENABLE;
    int res = ds3231_init(&_dev, &params);
    if (res != 0) {
        LOG_ERROR("unable to initialize DS3231 [I2C initialization error]");
        return -1;
    }

    return 0;
#endif

    // No RTC
    LOG_WARNING("Missing RTC module");
    return -1;
}


int wsn_rtc_temp(int16_t *temp) {
#ifdef MODULE_DS3231
    int res = ds3231_get_temp(&_dev, temp);
    if (res != 0) {
        LOG_ERROR("unable to read temperature");
        return -1;
    }

    return 0;
#endif

    // No RTC
    (void)temp;
    LOG_WARNING("Missing RTC module");
    return -1;
}


int wsn_rtc_time_get(struct tm *time)
{
#ifdef MODULE_DS3231
    int res = ds3231_get_time(&_dev, time);
    if (res != 0) {
        LOG_ERROR("unable to read time");
        return -1;
    }

    return 0;
#endif

    // No RTC
    (void)time;
    LOG_WARNING("Missing RTC module");
    return -1;
}


int wsn_rtc_time_set(struct tm *time)
{
#ifdef MODULE_DS3231
    int res = ds3231_set_time(&_dev, time);
    if (res != 0) {
        LOG_ERROR("unable to set time");
        return 1;
    }

    return 0;
#endif

    // No RTC
    (void)time;
    LOG_WARNING("Missing RTC module");
    return -1;
}
