/*
 * Copyright (C) 2022 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_gps gps
 * @ingroup     sys
 * @brief       GPS
 *
 * @{
 *
 * @file
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef GPS_H
#define GPS_H

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

bool gps_handle_gga(const char *line);
bool gps_handle_gll(const char *line);
bool gps_handle_gsa(const char *line);
bool gps_handle_gst(const char *line);
bool gps_handle_gsv(const char *line);
bool gps_handle_vtg(const char *line);
bool gps_handle_zda(const char *line);
bool gps_handle_rmc(const char *line);

#ifdef __cplusplus
}
#endif

#endif /* GPS_H */
/** @} */
