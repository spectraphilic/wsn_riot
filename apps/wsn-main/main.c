#include <assert.h>

// Riot
#include <board.h>
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <saul_reg.h>
#include <timex.h>
#include <ztimer.h>

// Riot
#include "triage.h"
#include "wsn.h"


#ifdef MODULE_ZTIMER_MSEC
#define ZTIMER ZTIMER_MSEC
#define TICKS_PER_SEC MS_PER_SEC
#else
#define ZTIMER ZTIMER_USEC
#define TICKS_PER_SEC US_PER_SEC
#endif

#define SLEEP 15 // seconds


int main(void)
{
    uint8_t buffer[128];
    nanocbor_encoder_t enc;
    phydat_t res;

    LED0_ON;
    test_utils_interactive_sync();

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for %ds in every loop.\n", SLEEP);

    // Main loop
    for (unsigned int loop=0; ; loop++) {
        LOG_INFO("Loop=%u\n", loop);

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

        saul_reg_t *dev = saul_reg;
        int i = 0;
        while (dev) {
            printf("%02i: %s\t%s ", i, dev->name, saul_class_to_str(dev->driver->type));
            int dim = saul_reg_read(dev, &res);
            if (dim <= 0) {
                printf("ERROR\n");
            } else {
                nanocbor_fmt_uint(&enc, i);
                for (int j=0; j < dim; j++) {
                    int value = res.val[j];
                    printf("%d ", value);
                    nanocbor_fmt_int(&enc, value);
                }
                printf("unit=%s scale=%d\n", phydat_unit_to_str(res.unit), res.scale);
            }

            // Next
            dev = dev->next;
            i++;
        }

        nanocbor_fmt_end_indefinite(&enc);
        size_t required = nanocbor_encoded_len(&enc);
        assert(required);

        printf("CBOR = ");
        for (size_t k=0; k < required; k++) {
            printf("%02x", buffer[k]);
        }
        printf("\n");

        // Done
        LOG_INFO("Loop=%u DONE\n", loop);
        LED0_OFF;
        ztimer_sleep(ZTIMER, SLEEP * TICKS_PER_SEC);
        LED0_ON;
    }

    return 0;
}
