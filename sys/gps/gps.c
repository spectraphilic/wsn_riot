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

#include <fmt.h>
#include <minmea.h>
#include <periph/uart.h>

#include "gps.h"


void gps_print_line(const char *prefix, char *line)
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

/*
static char* gps_set_checksum(char *cmd)
{
    int len = strlen(cmd);
    char checksum = gps_get_checksum(cmd);

    uint8_t aux = checksum / 16;
    if (aux < 10) {
        cmd[len-2] = aux + '0';
    }
    else {
        cmd[len-2] = aux + ('A' - 10);
    }

    aux = checksum % 16;
    if (aux < 10) {
        cmd[len-1] = aux + '0';
    }
    else {
        cmd[len-1] = aux + ('A' - 10);
    }

    return cmd;
}
*/

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

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "PSRF104,%s,%s,%s,%s,%s,%s,%s,%s", lat, lon, alt,
        clkOffset,
        timeOfWeek,
        weekNo,
        channel,
        resetCfg
    );

    gps_send_command(uart, cmd);
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
        printf(
            "RMC lat=%f long=%f speed=%f\n",
            minmea_tocoord(&frame.latitude),
            minmea_tocoord(&frame.longitude),
            minmea_tofloat(&frame.speed)
        );
    }

    return ok;
}
