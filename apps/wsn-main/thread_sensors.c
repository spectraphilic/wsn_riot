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

#if IS_USED(MODULE_FRAMES)
int send_frame(void)
{
    uint8_t buffer[150];
    uint8_t len;

    // Load frame
    int n = frames_load(buffer, &len);
    if (n <= 0)
        return n;

    // Send frame
    int error = wsn_network_send(buffer, len);
    if (error < 0)
        return error;

    LOG_INFO("Frame sent (%d left)", n);
    return n;
}
#endif

static void *task_func(void *arg)
{
    (void)arg;

    LOG_INFO("Running sensors thread, loop every %d seconds.", LOOP_SECONDS);

    uint8_t buffer[150];
    nanocbor_encoder_t enc;
#if IS_USED(MODULE_QTPY)
    phyval_t res;
#endif

    for (unsigned int loop=0; ; loop++) {
        LED0_ON;
        ztimer_now_t time = wsn_time_get(NULL);

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

#if IS_USED(MODULE_SX127X)
        // Source address
        uint8_t address[GNRC_NETIF_L2ADDR_MAXLEN];
        wsn_network_get_opt(NULL, NETOPT_ADDRESS, &address, sizeof(address));
        nanocbor_fmt_uint(&enc, 0);
        nanocbor_fmt_uint(&enc, address[0]);

        // Destination address
        nanocbor_fmt_uint(&enc, 1);
        nanocbor_fmt_uint(&enc, 1); // Send to gateway
#endif

        // Serial number
        nanocbor_fmt_uint(&enc, 2);
        nanocbor_fmt_uint(&enc, cpuid);

        // Name (Node Identifier)
        nanocbor_fmt_uint(&enc, 3);
        nanocbor_put_tstr(&enc, NODE_ID);

        // Frame sequence number
        nanocbor_fmt_uint(&enc, 4);
        nanocbor_fmt_uint(&enc, loop);

        // Timestamp
        nanocbor_fmt_uint(&enc, 123);
        nanocbor_fmt_uint(&enc, time);

#if IS_USED(MODULE_QTPY)
        // Sensors
        sensor_t *sensor = sensors_list;
        while (sensor) {
            printf("%s:\n", sensor->name);
            while (sensor->read(sensor->dev, &res)) {
                printf("%6ld unit=%-2s scale=%d\n",
                       res.value, phydat_unit_to_str(res.unit), res.scale);
                nanocbor_fmt_int(&enc, res.value);
            }
            sensor = sensor->next;
        }
#endif

        nanocbor_fmt_end_indefinite(&enc);
        size_t len = nanocbor_encoded_len(&enc);
        if (len == 0) {
            // TODO
        }

        // For debugging purposes, print CBOR data
        if (len < 30) {
            char message[90];
            sprintf(message, "loop=%d CBOR=", loop);
            for (size_t k=0; k < len; k++) {
                sprintf(message + strlen(message), "%02x", buffer[k]);
            }
            LOG_INFO("%s", message);
        } else {
            LOG_INFO("loop=%d CBOR size=%d", loop, len);
        }

#if IS_USED(MODULE_FRAMES)
        // Save the frame
        frames_save(time, buffer, len);

        // Send frames
        if (loop % SEND_LOOPS == 0) {
            send_frame();
        }
#endif

        // Done
        LED0_OFF;
#if IS_USED(MODULE_DS3231_INT)
        wsn_rtc_alarm_set(LOOP_SECONDS);
#else
        ztimer_sleep(ZTIMER_MSEC, LOOP_SECONDS * MS_PER_SEC);
#endif
    }

    // Never reached
    return NULL;
}

kernel_pid_t sensors_start(void)
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
