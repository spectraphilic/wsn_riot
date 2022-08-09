/*
 * Copyright (C) 2022 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_gps
 * @{
 *
 * @file
 * @brief       gps implementation
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#include <string.h>

#include <board.h>
#include <fmt.h>
#include <minmea.h>
#include <periph/gpio.h>
#include <periph/uart.h>
#include <ringbuffer.h>

#include <log.h>
#include "gps.h"


static bool __valid;
static struct minmea_time __time;
static struct minmea_float __latitude;
static struct minmea_float __longitude;
static struct minmea_float __speed;
static struct minmea_date __date;

static char rx_mem[128];
static ringbuffer_t rx_buf;

#define GPS_TASK_PRIO        (THREAD_PRIORITY_MAIN - 1)
static kernel_pid_t gps_task_pid;
static char gps_task_stack[THREAD_STACKSIZE_MAIN];

static void rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    ringbuffer_add_one(&rx_buf, data);

    if (data == '\n') {
        msg_t msg;
        msg_send(&msg, gps_task_pid);
    }
}

int gps_on(uart_t uart)
{
    int err;

#ifdef CPU_ATMEGA1281
    // Select GPS
    SET_MUX_GPS;

    // Switch on
    err = gpio_init(GPS_PW, GPIO_OUT);
    if (err != 0) {
        LOG_ERROR("GPS gpio_init failed err=%d\n", err);
        return -1;
    }
    gpio_set(GPS_PW);
#endif

    // Initialize ringbuffer
    ringbuffer_init(&rx_buf, rx_mem, sizeof(rx_mem));

    // Start the GPS thread
    gps_task_pid = thread_create(
        gps_task_stack,
        sizeof(gps_task_stack),
        GPS_TASK_PRIO,
        0,
        gps_task,
        (void*)&rx_buf,
        "gps"
    );

    // Init UART
    err = uart_init(uart, 4800, rx_cb, NULL); // 4800 bauds
    if (err != UART_OK) {
        LOG_ERROR("GPS uart_init failed err=%d\n", err);
        gps_off();
        return -1;
    }
    LOG_INFO("GPS on\n");

    return 0;
}

void gps_off(void)
{
#ifdef CPU_ATMEGA1281
//  uart_poweroff(uart);
    gpio_clear(GPS_PW);
    LOG_INFO("GPS off\n");
#endif
}

void gps_print_data(void)
{
    printf(
        "GPS lat=%f long=%f speed=%f\n",
        minmea_tocoord(&__latitude),
        minmea_tocoord(&__longitude),
        minmea_tofloat(&__speed)
    );
}

void gps_print_line(const char *prefix, const char *line)
{
    print_str(prefix);
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

static uint8_t gps_get_checksum(const char *cmd)
{
    // A GPS sentence looks like $<cmd>*cc where cc are the 2 checksum bytes.
    // The checksum is calculated by XOR of the content bytes <cmd>.
    uint8_t checksum = 0;
    for (uint16_t i=0; i < strlen(cmd); i++) {
        checksum ^= cmd[i];
    }
    return checksum;
}

static void gps_send_command(uart_t uart, char *cmd)
{
    uint8_t checksum = gps_get_checksum(cmd);

    char checksum_hex[3] = {0};
    fmt_byte_hex(checksum_hex, checksum);

    char line[128];
    snprintf(line, sizeof(line), "$%s*%s\r\n", cmd, checksum_hex);

    gps_print_line("TX ", line);

    uart_write(uart, (uint8_t*)line, strlen(line));
}

void gps_send_init_lla(uart_t uart)
{
    // TODO These should be input paramaters

//  // Svalbard
//  const char *lat = "78.9245075";
//  const char *lon = "11.9302955";
//  // Zaragoza
//  const char *lat = "41.680617";
//  const char *lon = "-0.886233";
//  const char *alt = "222";
    // Alcotas
    const char *lat = "40.013177";
    const char *lon = "-0.785576";
    const char *alt = "1000";

    const char *clkOffset = "96000";

    // 2022-08-05T12:00
    const char *timeOfWeek = "475200";
    const char *weekNo = "2221";

    const char *channel = "12";
    const char *resetCfg = "1";

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "PSRF104,%s,%s,%s,%s,%s,%s,%s,%s", lat, lon, alt,
        clkOffset,
        timeOfWeek,
        weekNo,
        channel,
        resetCfg
    );

    gps_send_command(uart, cmd);
}

int gps_handle(const char *line)
{
    gps_print_line("RX ", line);

    int ok = 1;
    int id = minmea_sentence_id(line, false);
    switch (id) {
        case MINMEA_INVALID:
            LOG_ERROR("MINMEA_INVALID\n");
            return -1;
        case MINMEA_UNKNOWN:
            LOG_ERROR("MINMEA_UNKNOWN\n");
            return -1;
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
            LOG_ERROR("MINMEA UNEXPECTED\n");
            return -1;
    }

    if (!ok) {
        LOG_ERROR("Failed to parse GPS sentence id=%d\n", id);
        return -1;
    }

    return 0;
}

bool gps_handle_gga(const char *line)
{
    // GGA - Global Positioning System Fix Data
    struct minmea_sentence_gga frame;
    int ok = minmea_parse_gga(&frame, line);
    if (ok) {
        printf("GGA fix_quality: %d\n", frame.fix_quality);
    }

    return ok;
}

bool gps_handle_gll(const char *line)
{
    // GLL - Geographic Position - Latitude/Longitude
    struct minmea_sentence_gll frame;
    int ok = minmea_parse_gll(&frame, line);
    if (ok) {
    }

    return ok;
}

bool gps_handle_gsa(const char *line)
{
    // GSA - GPS DOP and active satellites
    struct minmea_sentence_gsa frame;
    int ok = minmea_parse_gsa(&frame, line);
    if (ok) {
    }

    return ok;
}

bool gps_handle_gst(const char *line)
{
    // GST - GPS Pseudorange Noise Statistics
    struct minmea_sentence_gst frame;
    int ok = minmea_parse_gst(&frame, line);
    if (ok) {
    }

    return ok;
}

bool gps_handle_gsv(const char *line)
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

bool gps_handle_vtg(const char *line)
{
    // VTG - Track made good and Ground speed
    struct minmea_sentence_vtg frame;
    int ok = minmea_parse_vtg(&frame, line);
    if (ok) {
    }

    return ok;
}

bool gps_handle_zda(const char *line)
{
    // ZDA - Time & Date - UTC, day, month, year and local time zone
    struct minmea_sentence_zda frame;
    int ok = minmea_parse_zda(&frame, line);
    if (ok) {
    }

    return ok;
}

bool gps_handle_rmc(const char *line)
{
    // RMC - Recommended Minimum Navigation Information
    struct minmea_sentence_rmc frame;
    int ok = minmea_parse_rmc(&frame, line);
    if (ok) {
        if (frame.valid) {
            __valid = true;
            __date = frame.date;
            __time = frame.time;
            __latitude = frame.latitude;
            __longitude = frame.longitude;
            __speed = frame.speed;
        }
        else {
            LOG_WARNING("GPS Invalid RMC frame\n");
        }
    }

    return ok;
}

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

void *gps_task(void *arg)
{
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    ringbuffer_t *rx_buf = (ringbuffer_t*)arg;
    char line[128];

    while (1) {
        msg_receive(&msg);
        ringbuffer_get_line(rx_buf, line, sizeof(line));
        gps_handle(line);
    }

    // This should never be reached
    return NULL;
}
