/*
 * Copyright (C) 2022 UiO
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#include <board.h>
#include <periph/gpio.h>
#include <periph/uart.h>
#include <ps.h>
#include <ringbuffer.h>
#include <timex.h>
#include <ztimer.h>


//#define BAUDS 115200
#define BAUDS 4800

static char rx_mem[128];
static ringbuffer_t rx_buf;

static void print(void)
{
    char c;

    printf("RX ");
    do {
        c = (int)ringbuffer_get_one(&rx_buf);
        if (c == '\n') {
            printf("\\n");
        }
        else if (c == '\r') {
            printf("\\r");
        }
        else if (c >= ' ' && c <= '~') {
            printf("%c", c);
        }
        else {
            printf("0x%02x", (unsigned char)c);
        }
    } while (c != '\n');
    puts("");
}

/*
#define PRINTER_PRIO        (THREAD_PRIORITY_MAIN - 1)
static kernel_pid_t printer_pid;
static char printer_stack[THREAD_STACKSIZE_MAIN];
static void *printer(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        print();
    }

    // This should never be reached
    return NULL;
}
*/

static void rx_cb(void *arg, uint8_t data)
{
    (void)arg;

    ringbuffer_add_one(&rx_buf, data);

    if (data == '\n') {
        print();
        //msg_t msg;
        //msg_send(&msg, printer_pid);
    }
}

int main(void)
{
    int ret = 0;

    puts("Test GPS...");

    // Initialize ringbuffer
    ringbuffer_init(&rx_buf, rx_mem, sizeof(rx_mem));

//  // Start the printer thread
//  printer_pid = thread_create(printer_stack, sizeof(printer_stack),
//                              PRINTER_PRIO, 0, printer, NULL, "printer");

    // Select GPS
    SET_MUX_GPS;
    puts("[OK] SET_MUX_GPS");

    // Switch on
    int err = gpio_init(GPS_PW, GPIO_OUT);
    if (err != 0) {
        printf("ERROR gpio_init %d\n", err);
        return -1;
    }
    gpio_set(GPS_PW);
    puts("[OK] GPS on");

    // Init UART
    uart_t uart = UART_DEV(1);
    err = uart_init(uart, BAUDS, rx_cb, NULL);
    if (err != UART_OK) {
        printf("ERROR uart_init %d\n", err);
        ret = -1;
        goto exit;
    }
    puts("[OK] UART init");

//  ps();

    ztimer_sleep(ZTIMER_USEC, 10 * US_PER_SEC);
//  uart_poweroff(uart);
//  puts("[OK] UART off");

exit:
    // Switch off
    gpio_clear(GPS_PW);
    puts("[OK] GPS off");

    return ret;
}
