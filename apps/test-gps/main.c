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
#include <ps.h>
#include <ringbuffer.h>
#include <timex.h>
#include <ztimer.h>

#include <gps.h>


int main(void)
{
    puts("Test GPS...");

    uart_t uart = UART_DEV(1);
    gps_start_loop(uart);

    ps();
    thread_sleep(); // Sleep main thread

    return 0;
}
