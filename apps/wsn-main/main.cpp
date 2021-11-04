// Riot
#include <board.h>
#include <log.h>
#include <ztimer.h>

// Project
#include <wsn.h>
#include "common.h"

int main(void)
{
    LED0_ON;

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("basetime=%d", wsn_time_basetime());

    // Start thread that handles incoming packets
    thread_recv_start();

    // Get system time from the gateway if not set already
    const char msg[] = "ping";
    const uint8_t size = strlen(msg);
    while (wsn_time_basetime() == 0) {
        send_data((uint8_t*)msg, size);
        LOG_INFO(msg);
        ztimer_sleep(ZTIMER, 10 * TICKS_PER_SEC);
    }

    // Start other threads
    thread_sensors_start();
    thread_send_start();

    // Main thread exits, the program will run while there're threads alive (forever)
    return 0;
}
