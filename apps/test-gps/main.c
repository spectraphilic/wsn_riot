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


static char rx_mem[128];
static ringbuffer_t rx_buf;

static unsigned ringbuffer_get_line(ringbuffer_t *rb, char *buf, unsigned bufsize)
{
    unsigned i = 0;
    while (i < bufsize - 1) {
        int c = ringbuffer_get_one(rb);
        if (c < 0) {
            break;
        }
        buf[i++] = (char)c;
        if (c == '\n') {
            break;
        }
    }

    buf[i] = '\0';
    return i;
}

static void handle(void)
{
    char line[128];
    int ok = 1;
    ringbuffer_get_line(&rx_buf, line, sizeof(line));

    gps_print_line("RX ", line);

    int id = minmea_sentence_id(line, false);
    switch (id) {
        case MINMEA_INVALID:
            puts("MINMEA_INVALID");
            break;
        case MINMEA_UNKNOWN:
            puts("MINMEA_UNKNOWN");
            break;
        case MINMEA_SENTENCE_RMC:
            ok = gps_handle_rmc(line);
            break;
        case MINMEA_SENTENCE_GGA:
            ok = gps_handle_gga(line);
            break;
        case MINMEA_SENTENCE_GSA:
            ok = gps_handle_gsa(line);
            break;
        case MINMEA_SENTENCE_GLL:
            ok = gps_handle_gll(line);
            break;
        case MINMEA_SENTENCE_GST:
            ok = gps_handle_gst(line);
            break;
        case MINMEA_SENTENCE_GSV:
            ok = gps_handle_gsv(line);
            break;
        case MINMEA_SENTENCE_VTG:
            ok = gps_handle_vtg(line);
            break;
        case MINMEA_SENTENCE_ZDA:
            ok = gps_handle_zda(line);
            break;
        default:
            puts("MINMEA UNEXPECTED");
    }

    if (!ok) {
        puts("Error parsing frame");
    }
}

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
        handle();
    }

    // This should never be reached
    return NULL;
}

static void rx_cb(void *arg, uint8_t data)
{
    ringbuffer_add_one(&rx_buf, data);
    kernel_pid_t pid = *(kernel_pid_t*)arg;

    if (data == '\n') {
        msg_t msg;
        msg_send(&msg, pid);
    }
}

int main(void)
{
    int ret = 0;

    puts("Test GPS...");

    // Initialize ringbuffer
    ringbuffer_init(&rx_buf, rx_mem, sizeof(rx_mem));

    // Start the printer thread
    printer_pid = thread_create(printer_stack, sizeof(printer_stack),
                                PRINTER_PRIO, 0, printer, NULL, "printer");

    uart_t uart = UART_DEV(1);
    int err = gps_on(uart, rx_cb, &printer_pid);
    if (err == 0) {
        ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
        gps_send_init_lla(uart);

        ztimer_sleep(ZTIMER_USEC, 5 * US_PER_SEC);
    }

    gps_off();

    return ret;
}
