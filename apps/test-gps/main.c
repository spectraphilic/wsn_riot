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
#include <string.h>

#include <board.h>
#include <fmt.h>
#include <minmea.h>
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

static void print_line(char *line)
{
    printf("RX ");
    for (unsigned i=0; i < strlen(line); i++) {
        char c = line[i];
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
    }
    puts("");
}

static bool handle_gga(const char *line)
{
    struct minmea_sentence_gga frame;
    int ok = minmea_parse_gga(&frame, line);
    if (ok) {
        puts("GGA ok");
    }

    return ok;
}

static bool handle_gll(const char *line)
{
    struct minmea_sentence_gll frame;
    int ok = minmea_parse_gll(&frame, line);
    if (ok) {
        puts("GLL ok");
    }

    return ok;
}

static bool handle_gsa(const char *line)
{
    struct minmea_sentence_gsa frame;
    int ok = minmea_parse_gsa(&frame, line);
    if (ok) {
        puts("GSA ok");
    }

    return ok;
}

static bool handle_gst(const char *line)
{
    struct minmea_sentence_gst frame;
    int ok = minmea_parse_gst(&frame, line);
    if (ok) {
        puts("GST ok");
    }

    return ok;
}

static bool handle_gsv(const char *line)
{
    struct minmea_sentence_gsv frame;
    int ok = minmea_parse_gsv(&frame, line);
    if (ok) {
        puts("GSV ok");
    }

    return ok;
}

static bool handle_vtg(const char *line)
{
    struct minmea_sentence_vtg frame;
    int ok = minmea_parse_vtg(&frame, line);
    if (ok) {
        puts("VTG ok");
    }

    return ok;
}

static bool handle_zda(const char *line)
{
    struct minmea_sentence_zda frame;
    int ok = minmea_parse_zda(&frame, line);
    if (ok) {
        puts("ZDA ok");
    }

    return ok;
}

static bool handle_rmc(const char *line)
{
    struct minmea_sentence_rmc frame;
    int ok = minmea_parse_rmc(&frame, line);
    if (ok) {
        puts("RMC ok");
    }

    return ok;
}

static void handle(void)
{
    char line[128];
    int ok;
    ringbuffer_get_line(&rx_buf, line, sizeof(line));

    print_line(line);

    int id = minmea_sentence_id(line, false);
    switch (id) {
        case MINMEA_INVALID:
            puts("MINMEA_INVALID");
            break;
        case MINMEA_UNKNOWN:
            puts("MINMEA_UNKNOWN");
            break;
        case MINMEA_SENTENCE_RMC:
            ok = handle_rmc(line);
            if (!ok) {
                puts("RMC error");
            }
            break;
        case MINMEA_SENTENCE_GGA:
            ok = handle_gga(line);
            if (!ok) {
                puts("GGA error");
            }
            break;
        case MINMEA_SENTENCE_GSA:
            ok = handle_gsa(line);
            if (!ok) {
                puts("GSA error");
            }
            break;
        case MINMEA_SENTENCE_GLL:
            ok = handle_gll(line);
            if (!ok) {
                puts("GLL error");
            }
            break;
        case MINMEA_SENTENCE_GST:
            ok = handle_gst(line);
            if (!ok) {
                puts("GST error");
            }
            break;
        case MINMEA_SENTENCE_GSV:
            ok = handle_gsv(line);
            if (!ok) {
                puts("GSV error");
            }
            break;
        case MINMEA_SENTENCE_VTG:
            ok = handle_vtg(line);
            if (!ok) {
                puts("VTG error");
            }
            break;
        case MINMEA_SENTENCE_ZDA:
            ok = handle_zda(line);
            if (!ok) {
                puts("ZDA error");
            }
            break;
        default:
            puts("MINMEA UNEXPECTED");
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
    (void)arg;

    ringbuffer_add_one(&rx_buf, data);

    if (data == '\n') {
        msg_t msg;
        msg_send(&msg, printer_pid);
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
