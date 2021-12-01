#ifdef MODULE_NANOCBOR

// Riot
#include <board.h>
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <thread.h>
#include <ztimer.h>

// Project
#include <frames.h>
#include <sensors.h>
#include <triage.h>
#include <wsn.h>
#include "common.h"
#include "config.h"

static kernel_pid_t pid = KERNEL_PID_UNDEF;
static char stack[THREAD_STACKSIZE_MAIN];

int send_frame(void)
{
    uint8_t buffer[150];
    uint8_t len;

    // Load frame
    int n = frames_load(buffer, &len);
    if (n <= 0)
        return n;

    // Send frame
    int error = send_data(buffer, len);
    if (error < 0)
        return error;

    LOG_INFO("Frame sent (%d left)", n);
    return n;
}

static void *task_func(void *arg)
{
    (void)arg;

    LOG_INFO("Running sensors thread, loop every %d seconds.", LOOP_SECONDS);

    uint8_t buffer[150];
    nanocbor_encoder_t enc;
    phyval_t res;

    for (unsigned int loop=0; ; loop++) {
        LED0_ON;
        ztimer_now_t time = wsn_time_get(NULL);

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
        if (len == 0) {
            // TODO
        }

        // For debugging purposes, print CBOR data
        if (len < 30) {
            char message[80] = "CBOR=";
            for (size_t k=0; k < len; k++) {
                sprintf(message + strlen(message), "%02x", buffer[k]);
            }
            LOG_INFO("%s", message);
        } else {
            LOG_INFO("CBOR size=%d", len);
        }

        // Save the frame
        frames_save(time, buffer, len);

        // Send frames
        if (loop % SEND_LOOPS == 0) {
            send_frame();
        }

        // Done
        LED0_OFF;
        ztimer_sleep(ZTIMER_MSEC, LOOP_SECONDS * MS_PER_SEC);
    }

    // Never reached
    return NULL;
}

kernel_pid_t thread_sensors_start(void)
{
    if (pid == KERNEL_PID_UNDEF) {
        pid = thread_create(
            stack,
            sizeof(stack),
            THREAD_PRIORITY_SENSORS,
            THREAD_CREATE_STACKTEST,
            task_func,
            NULL,
            "sensors"
        );

        if (pid < 0) {
            LOG_ERROR("Failed to create thread %s", errno_string(pid));
            return pid;
        }
    }

    return pid;
}

#endif
