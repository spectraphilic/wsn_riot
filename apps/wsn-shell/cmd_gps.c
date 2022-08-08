#include <periph/uart.h>
#include <timex.h>
#include <ztimer.h>

#include <gps.h>
#include <triage.h>


int cmd_gps(int argc, char **argv)
{
    // Arguments
    if (check_argc(argc, 1) < 0) {
        return -1;
    }
    (void)argv;

    // Switch on
    uart_t uart = UART_DEV(1);
    int err = gps_on(uart, NULL);
    if (err != 0) {
        return -1;
    }

    // Initialize
    ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
    gps_send_init_lla(uart);

    // Wait
    ztimer_sleep(ZTIMER_USEC, 5 * US_PER_SEC);

    // Switch off
    gps_off();

    return 0;
}
