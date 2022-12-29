#ifdef MODULE_DS3231

#include <ds3231.h>
#include <ds3231_params.h>
#include <log.h>

static ds3231_t dev;

int wsn_rtc_init(void)
{
    ds3231_params_t params= ds3231_params[0];
    params.opt     = DS3231_OPT_BAT_ENABLE;
    params.opt    |= DS3231_OPT_INTER_ENABLE;
    int err = ds3231_init(&dev, &params);
    if (err) {
        LOG_ERROR("DS3231 init failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_temp(int16_t *temp) {
    int err = ds3231_get_temp(&dev, temp);
    if (err) {
        LOG_ERROR("DS3231 get temp failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_time_get(struct tm *time)
{
    int err = ds3231_get_time(&dev, time);
    if (err) {
        LOG_ERROR("DS3231 get time failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_time_set(struct tm *time)
{
    int err = ds3231_set_time(&dev, time);
    if (err) {
        LOG_ERROR("DS3231 set time failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_alarm_set(int delay) // TODO Change to exact time
{
    int err;
    struct tm time;

    err = ds3231_clear_alarm_1_flag(&dev);
    if (err) {
        LOG_ERROR("DS3231 clear alarm flag failed err=%d", err);
        return -1;
    }

    err = ds3231_get_time(&dev, &time);
    if (err) {
        LOG_ERROR("DS3231 get time failed err=%d", err);
        return -1;
    }

    time.tm_sec += delay;
    mktime(&time);
    err = ds3231_set_alarm_1(&dev, &time, DS3231_AL1_TRIG_H_M_S);
    if (err) {
        LOG_ERROR("DS3231 set alarm failed err=%d", err);
        return -1;
    }

    err = ds3231_await_alarm(&dev);
    if (err < 0) { // Returns flag on success (alarm 1 or 2)
        LOG_ERROR("DS3231 await alarm failed err=%d", err);
        return -1;
    }

    return 0;
}

#endif
