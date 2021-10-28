#include <assert.h>

// Riot
#include <board.h>
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <thread.h>

// Project
#include <frames.h>
#include <sensors.h>
#include <wsn.h>
#include "common.h"
#include "config.h"

static kernel_pid_t pid = KERNEL_PID_UNDEF;
static char stack[THREAD_STACKSIZE_MAIN];

static void *task_func(void *arg)
{
    (void)arg;

    LOG_INFO("Running sensors thread, loop every %d seconds.", LOOP_SECONDS);

    uint8_t buffer[150];
    nanocbor_encoder_t enc;
    phyval_t res;

    for (unsigned int loop=0; ; loop++) {
        LED0_ON;
        time_t time = wsn_time_get();

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

        // Timestamp
        nanocbor_fmt_uint(&enc, 0);
        nanocbor_fmt_uint(&enc, time);

        // Serial number
        nanocbor_fmt_uint(&enc, 1);
        nanocbor_fmt_uint(&enc, cpuid);

        // Name (Node Identifier)
        nanocbor_fmt_uint(&enc, 2);
        nanocbor_put_tstr(&enc, NODE_ID);

        // Frame sequence number
        nanocbor_fmt_uint(&enc, 3);
        nanocbor_fmt_uint(&enc, loop);

        sensor_t *sensor = sensors_list;
        while (sensor) {
            printf("%s:\n", sensor->name);
            while (sensor->read(sensor->dev, &res)) {
                printf("%6ld unit=%-2s scale=%d\n", res.value, phydat_unit_to_str(res.unit), res.scale);
                nanocbor_fmt_int(&enc, res.value);
            }
            sensor = sensor->next;
        }

        nanocbor_fmt_end_indefinite(&enc);
        size_t len = nanocbor_encoded_len(&enc);
        assert(len);

        // For debugging purposes, print CBOR data
        char message[150] = "CBOR=";
        for (size_t k=0; k < len; k++) {
            sprintf(message + strlen(message), "%02x", buffer[k]);
        }
        LOG_INFO(message);

        // Save the frame
        frames_save(time, buffer, len);
        LOG_INFO("Frame saved len=%d", len);

        // Done
        LED0_OFF;
        ztimer_sleep(ZTIMER, LOOP_SECONDS * TICKS_PER_SEC);
    }

    // Never reached
    return NULL;
}

void thread_sensors_start(void)
{
    pid = thread_create(
        stack,
        sizeof(stack),
        THREAD_PRIORITY_SENSORS,
        THREAD_CREATE_STACKTEST,
        task_func,
        NULL,
        "sensors"
    );
}
