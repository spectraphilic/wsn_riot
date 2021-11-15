// Riot
#include <board.h>
#include <log.h>
#include <net/gnrc.h>
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
    LOG_INFO("basetime=%lld", wsn_time_basetime());

    // Start thread that handles incoming packets
    kernel_pid_t pid = thread_recv_start();
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    // Get system time from the gateway if not set already
    const char msg[] = "ping";
    const uint8_t size = strlen(msg);
    while (wsn_time_basetime() == 0) {
        send_data((uint8_t*)msg, size);
        LOG_INFO(msg);
        ztimer_sleep(ZTIMER_MSEC, 10 * MS_PER_SEC);
    }

    LED0_OFF;

    // Start other threads
    thread_sensors_start();
    thread_send_start();

    // The main thread will sleep forever
    thread_sleep();
    return 0;
}
