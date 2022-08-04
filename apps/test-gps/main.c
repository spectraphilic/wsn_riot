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
    print_str("RX ");
    for (unsigned i=0; i < strlen(line); i++) {
        char c = line[i];
        if (c == '\n') {
            print_str("\\n");
        }
        else if (c == '\r') {
            print_str("\\r");
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
    // GGA - Global Positioning System Fix Data
    struct minmea_sentence_gga frame;
    int ok = minmea_parse_gga(&frame, line);
    if (ok) {
        printf("GGA fix_quality: %d\n", frame.fix_quality);
    }

    return ok;
}

static bool handle_gll(const char *line)
{
    // GLL - Geographic Position - Latitude/Longitude
    struct minmea_sentence_gll frame;
    int ok = minmea_parse_gll(&frame, line);
    if (ok) {
    }

    return ok;
}

static bool handle_gsa(const char *line)
{
    // GSA - GPS DOP and active satellites
    struct minmea_sentence_gsa frame;
    int ok = minmea_parse_gsa(&frame, line);
    if (ok) {
    }

    return ok;
}

static bool handle_gst(const char *line)
{
    // GST - GPS Pseudorange Noise Statistics
    struct minmea_sentence_gst frame;
    int ok = minmea_parse_gst(&frame, line);
    if (ok) {
    }

    return ok;
}

static bool handle_gsv(const char *line)
{
    // GSV - Satellites in view
    struct minmea_sentence_gsv frame;
    int ok = minmea_parse_gsv(&frame, line);
    if (ok) {
        printf("GSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
        printf("GSV: satellites in view: %d\n", frame.total_sats);
        for (int i = 0; i < 4; i++)
            printf("GSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                frame.sats[i].nr,
                frame.sats[i].elevation,
                frame.sats[i].azimuth,
                frame.sats[i].snr);
    }

    return ok;
}

static bool handle_vtg(const char *line)
{
    // VTG - Track made good and Ground speed
    struct minmea_sentence_vtg frame;
    int ok = minmea_parse_vtg(&frame, line);
    if (ok) {
    }

    return ok;
}

static bool handle_zda(const char *line)
{
    // ZDA - Time & Date - UTC, day, month, year and local time zone
    struct minmea_sentence_zda frame;
    int ok = minmea_parse_zda(&frame, line);
    if (ok) {
    }

    return ok;
}

static bool handle_rmc(const char *line)
{
    // RMC - Recommended Minimum Navigation Information
    struct minmea_sentence_rmc frame;
    int ok = minmea_parse_rmc(&frame, line);
    if (ok) {
        printf(
            "RMC lat=%f long=%f speed=%f\n",
            minmea_tocoord(&frame.latitude),
            minmea_tocoord(&frame.longitude),
            minmea_tofloat(&frame.speed)
        );
    }

    return ok;
}

static void handle(void)
{
    char line[128];
    int ok = 1;
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
            break;
        case MINMEA_SENTENCE_GGA:
            ok = handle_gga(line);
            break;
        case MINMEA_SENTENCE_GSA:
            ok = handle_gsa(line);
            break;
        case MINMEA_SENTENCE_GLL:
            ok = handle_gll(line);
            break;
        case MINMEA_SENTENCE_GST:
            ok = handle_gst(line);
            break;
        case MINMEA_SENTENCE_GSV:
            ok = handle_gsv(line);
            break;
        case MINMEA_SENTENCE_VTG:
            ok = handle_vtg(line);
            break;
        case MINMEA_SENTENCE_ZDA:
            ok = handle_zda(line);
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
