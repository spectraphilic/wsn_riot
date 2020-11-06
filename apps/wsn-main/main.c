#include <assert.h>

// Riot
#include <board.h>
#include <log.h>
#include <saul_reg.h>
#include <timex.h>
#include <ztimer.h>

// Riot
#include "wsn.h"


#ifdef MODULE_ZTIMER_MSEC
#define ZTIMER ZTIMER_MSEC
#define TICKS_PER_SEC MS_PER_SEC
#else
#define ZTIMER ZTIMER_USEC
#define TICKS_PER_SEC US_PER_SEC
#endif

#define SLEEP 5 * TICKS_PER_SEC // 5 seconds


static void print_value(phydat_t *data, uint8_t dim)
{
    if (data == NULL || dim > PHYDAT_DIM) {
        puts("Unable to display data object");
        return;
    }

    if (data->unit == UNIT_TIME) {
        assert(dim == 3);
        printf("%02d:%02d:%02d", data->val[2], data->val[1], data->val[0]);
        return;
    }
    if (data->unit == UNIT_DATE) {
        assert(dim == 3);
        printf("%04d-%02d-%02d", data->val[2], data->val[1], data->val[0]);
        return;
    }

    for (uint8_t i = 0; i < dim; i++) {
        char scale_prefix;

        switch (data->unit) {
            case UNIT_UNDEF:
            case UNIT_NONE:
            case UNIT_M2:
            case UNIT_M3:
            case UNIT_PERCENT:
            case UNIT_TEMP_C:
            case UNIT_TEMP_F:
            case UNIT_DBM:
                /* no string conversion */
                scale_prefix = '\0';
                break;
            default:
                scale_prefix = phydat_prefix_from_scale(data->scale);
        }

        if (dim > 1) {
            printf("[%u] ", (unsigned int)i);
        }
        else {
            printf("    ");
        }
        if (scale_prefix) {
            printf("%11d %c", (int)data->val[i], scale_prefix);
        }
        else if (data->scale == 0) {
            printf("%11d ", (int)data->val[i]);
        }
        else if ((data->scale > -6) && (data->scale < 0)) {
            char num[9];
            size_t len = fmt_s16_dfp(num, data->val[i], data->scale);
            assert(len < 9);
            num[len] = '\0';
            printf("%11s ", num);
        }
        else {
            char num[12];
            snprintf(num, sizeof(num), "%ie%i", (int)data->val[i], (int)data->scale);
            printf("%11s ", num);
        }

        printf("%s", phydat_unit_to_str(data->unit));
    }
}


static void read_dev(saul_reg_t *dev)
{
    int dim;
    phydat_t res;

    dim = saul_reg_read(dev, &res);
    if (dim <= 0) {
        printf("ERROR");
    } else {
        print_value(&res, dim);
    }

    printf("\n");
}


int main(void)
{
    LED0_ON;

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for 5s in every loop.");

    // Main loop
    for (unsigned int loop=0; ; loop++) {
        LOG_INFO("Loop=%u\n", loop);

        saul_reg_t *dev = saul_reg;
        int i = 0;
        while (dev) {
            printf("#%i\t%s\t%s\t", i++, saul_class_to_str(dev->driver->type), dev->name);
            read_dev(dev);
            dev = dev->next;
        }

        LOG_INFO("Loop=%u DONE\n", loop);
        LED0_OFF;
        ztimer_sleep(ZTIMER, SLEEP);
        LED0_ON;
    }

    return 0;
}
