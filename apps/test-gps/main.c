/*
 * Copyright (C) 2022 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Test GPS
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include <minmea.h>
#include <periph/uart.h>
#include <ringbuffer.h>
#include <timex.h>
#include <ztimer.h>

#include <gps.h>


int main(void)
{
    int ret = 0;

    puts("Test GPS...");

    uart_t uart = UART_DEV(1);
    int err = gps_on(uart);
    if (err == 0) {
        ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
        gps_send_init_lla(uart);

        ztimer_sleep(ZTIMER_USEC, 5 * US_PER_SEC);
    }

    gps_off();

    return ret;
}
