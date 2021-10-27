// Riot
#include <log.h>
#include <ztimer.h>

// Project
#include <wsn.h>
#include "common.h"
#include "config.h"


static kernel_pid_t pid = KERNEL_PID_UNDEF;
static char stack[THREAD_STACKSIZE_MAIN];

static int send_frame(void)
{
    uint8_t buffer[150];
    uint8_t len;

    // Load frame
    int n = wsn_load_frame(buffer, &len);
    if (n <= 0)
        return n;

    // Send frame
    return send_data(buffer, len);
}

static void *task_func(void *arg)
{
    (void)arg;

    LOG_INFO("Running sending thread, loop every %d seconds.", SEND_SECONDS);

    while (1) {
        // Send while there're frames to send
        LOG_INFO("Sending frames...");
        while (1) {
            int n = send_frame();
            if (n <= 0)
                break;

            // Drop frame
            n = wsn_drop_frame();
            if (n <= 0)
                break;
        }

        LOG_INFO("Sending frames DONE");
        ztimer_sleep(ZTIMER, SEND_SECONDS * TICKS_PER_SEC);
    }

    // Never reached
    return NULL;
}

void thread_send_start(void)
{
    pid = thread_create(
        stack,
        sizeof(stack),
        THREAD_PRIORITY_MAIN -1,
        THREAD_CREATE_STACKTEST,
        task_func,
        NULL,
        "send-frames"
    );
}
