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

#include <minmea.h>

#include "gps.h"


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
