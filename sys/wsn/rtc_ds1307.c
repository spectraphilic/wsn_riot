#ifdef MODULE_DS1307

#include <ds1307.h>
#include <ds1307_params.h>
#include <log.h>

static ds1307_t dev;

int wsn_rtc_init(void)
{
    ds1307_params_t params = ds1307_params[0];
    int err = ds1307_init(&dev, &params);
    if (err) {
        LOG_ERROR("DS1307 init failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_time_get(struct tm *time)
{
    int err = ds1307_get_time(&dev, time);
    if (err) {
        LOG_ERROR("DS1307 get time failed err=%d", err);
        return -1;
    }

    return 0;
}

int wsn_rtc_time_set(struct tm *time)
{
    int err = ds1307_set_time(&dev, time);
    if (err) {
        LOG_ERROR("DS1307 set time failed err=%d", err);
        return 1;
    }

    return 0;
}

#endif
