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

//#define ENABLE_DEBUG 1

#include <string.h>

#include <board.h>
#include <debug.h>
#include <fmt.h>
#include <minmea.h>
#include <periph/gpio.h>
#include <ringbuffer.h>
#include <timex.h>
#include <ztimer.h>

#include <log.h>
#include "gps.h"


static bool done;
static struct minmea_sentence_rmc rmc;

static char rx_mem[128];
static ringbuffer_t rx_buf;

static kernel_pid_t gps_parser_pid = 0;


static void rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    ringbuffer_add_one(&rx_buf, data);

    if (data == '\n') {
        msg_t msg;
        msg_send(&msg, gps_parser_pid);
    }
}


bool gps_done(void)
{
    return done;
}

int gps_on(uart_t uart)
{
    int err;
    uint32_t baudrate;

    done = false;

#ifdef CPU_ATMEGA1281
    baudrate = 4800;

    // Select GPS
    SET_MUX_GPS;

    // Switch on
    err = gpio_init(GPS_PW, GPIO_OUT);
    if (err != 0) {
        LOG_ERROR("GPS gpio_init failed err=%d\n", err);
        return -1;
    }
    gpio_set(GPS_PW);
#else
    // Grove - GPS (Air530) works at 9600 bauds by default
    baudrate = 9600;
#endif

    // Initialize ringbuffer
    ringbuffer_init(&rx_buf, rx_mem, sizeof(rx_mem));

    // Start the GPS thread
    gps_start_parser();

    // Init UART XXX
    err = uart_init(uart, baudrate, rx_cb, NULL);
    LOG_INFO("UART %d initialized bauds=%lu err=%d\n", uart, baudrate, err);
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
        minmea_tocoord(&rmc.latitude),
        minmea_tocoord(&rmc.longitude),
        minmea_tofloat(&rmc.speed)
    );
}

static void print_line(const char *prefix, const char *line)
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

static uint8_t get_checksum(const char *cmd)
{
    // A GPS sentence looks like $<cmd>*cc where cc are the 2 checksum bytes.
    // The checksum is calculated by XOR of the content bytes <cmd>.
    uint8_t checksum = 0;
    for (uint16_t i=0; i < strlen(cmd); i++) {
        checksum ^= cmd[i];
    }
    return checksum;
}

static void send_command(uart_t uart, char *cmd)
{
    uint8_t checksum = get_checksum(cmd);

    char checksum_hex[3] = {0};
    fmt_byte_hex(checksum_hex, checksum);

    char line[128];
    snprintf(line, sizeof(line), "$%s*%s\r\n", cmd, checksum_hex);

    print_line("TX ", line);

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

    send_command(uart, cmd);
}

static int handle_gga(const char *line)
{
    // GGA - Global Positioning System Fix Data
    struct minmea_sentence_gga frame;
    int ok = minmea_parse_gga(&frame, line);
    if (ok) {
        DEBUG("[gps] GGA fix_quality: %d\n", frame.fix_quality);
    }

    return ok;
}

static int handle_gll(const char *line)
{
    // GLL - Geographic Position - Latitude/Longitude
    struct minmea_sentence_gll frame;
    int ok = minmea_parse_gll(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_gsa(const char *line)
{
    // GSA - GPS DOP and active satellites
    struct minmea_sentence_gsa frame;
    int ok = minmea_parse_gsa(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_gst(const char *line)
{
    // GST - GPS Pseudorange Noise Statistics
    struct minmea_sentence_gst frame;
    int ok = minmea_parse_gst(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_gsv(const char *line)
{
    // GSV - Satellites in view
    struct minmea_sentence_gsv frame;
    int ok = minmea_parse_gsv(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_rmc(const char *line)
{
    // RMC - Recommended Minimum Navigation Information
    struct minmea_sentence_rmc frame;
    int ok = minmea_parse_rmc(&frame, line);
    if (ok) {
        if (frame.valid) {
            rmc = frame;
            done = true;
        }
        else {
            DEBUG("[gps] Invalid RMC frame\n");
        }
    }

    return ok;
}

static int handle_vtg(const char *line)
{
    // VTG - Track made good and Ground speed
    struct minmea_sentence_vtg frame;
    int ok = minmea_parse_vtg(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_zda(const char *line)
{
    // ZDA - Time & Date - UTC, day, month, year and local time zone
    struct minmea_sentence_zda frame;
    int ok = minmea_parse_zda(&frame, line);
    if (ok) {
    }

    return ok;
}

static int handle_sentence(const char *line)
{
    print_line("RX ", line);

    int ok = 1;
    int id = minmea_sentence_id(line, false);
    switch (id) {
        case MINMEA_INVALID:
            DEBUG("[gps] MINMEA_INVALID\n");
            return -1;
        case MINMEA_UNKNOWN:
            DEBUG("[gps] MINMEA_UNKNOWN\n");
            return -1;
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
            DEBUG("[gps] MINMEA UNEXPECTED\n");
            return -1;
    }

    if (!ok) {
        LOG_ERROR("Failed to parse GPS sentence id=%d\n", id);
        return -1;
    }

    return 0;
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

static void *gps_parser_task(void *arg)
{
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    ringbuffer_t *rx_buf = (ringbuffer_t*)arg;
    char line[128];

    while (1) {
        msg_receive(&msg);
        ringbuffer_get_line(rx_buf, line, sizeof(line));
        handle_sentence(line);
    }

    return NULL;
}

static char gps_parser_stack[THREAD_STACKSIZE_MAIN];
void gps_start_parser(void)
{
    if (gps_parser_pid == 0) {
        gps_parser_pid = thread_create(
            gps_parser_stack,
            sizeof(gps_parser_stack),
            THREAD_PRIORITY_MAIN - 2,
            0,
            gps_parser_task,
            (void*)&rx_buf,
            "gps_parser"
        );
    }
}


void gps_wait(ztimer_now_t timeout)
{
    bool done = false;
    ztimer_now_t now = ztimer_now(ZTIMER_USEC);
    while ((ztimer_now(ZTIMER_USEC) - now) < timeout) {
        done = gps_done();
        if (done) {
            break;
        }

        ztimer_sleep(ZTIMER_USEC, 2 * US_PER_SEC);
    }
}


static void *gps_loop_task(void *arg)
{
    // Switch on
    uart_t uart = *(uart_t*)arg;

    while (1) {
        int err = gps_on(uart);
        if (err != 0) {
            LOG_ERROR("GPS error initializing\n");
            return NULL;
        }

        // Initialize
        ztimer_sleep(ZTIMER_USEC, 1000 * US_PER_MS);
        gps_send_init_lla(uart);

        // Wait
        gps_wait(600 * US_PER_SEC);

        // Switch off
        gps_off();

        if (done) {
            LOG_DEBUG("GPS ok");
            gps_print_data();
        }
        else {
            LOG_WARNING("GPS timeout\n");
        }

        ztimer_sleep(ZTIMER_USEC, 300 * US_PER_SEC);
    }

    return NULL;
}

static char gps_loop_stack[THREAD_STACKSIZE_MAIN];
void gps_start_loop(uart_t uart)
{
    thread_create(
        gps_loop_stack,
        sizeof(gps_loop_stack),
        THREAD_PRIORITY_MAIN - 1,
        0,
        gps_loop_task,
        (void*)&uart,
        "gps_loop"
    );
}
