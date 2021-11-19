// Riot
#include <board.h>
#include <log.h>
#include <thread.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include <frames.h>
#include <triage.h>
#include <wsn.h>
#include "common.h"
#include "config.h"


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

    LOG_INFO("Frame sent n=%d");
    return n;
}

static void *task_func(void *arg)
{
    (void)arg;

    LOG_INFO("Send frames every %d seconds.", SEND_SECONDS);
    while (1) {
        LED1_ON;
        send_frame();
        LED1_OFF;
        ztimer_sleep(ZTIMER_MSEC, SEND_SECONDS * MS_PER_SEC);
    }

    // Never reached
    return NULL;
}

void thread_send_start(void)
{
    task_func(NULL);
}
