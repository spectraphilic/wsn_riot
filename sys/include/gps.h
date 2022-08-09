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

int gps_on(uart_t uart);
void gps_off(void);
void gps_print_data(void);
void gps_print_line(const char *prefix, const char *line);
int gps_handle(const char *line);
bool gps_handle_gga(const char *line);
bool gps_handle_gll(const char *line);
bool gps_handle_gsa(const char *line);
bool gps_handle_gst(const char *line);
bool gps_handle_gsv(const char *line);
bool gps_handle_vtg(const char *line);
bool gps_handle_zda(const char *line);
bool gps_handle_rmc(const char *line);
void gps_send_init_lla(uart_t uart);
void *gps_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* GPS_H */
/** @} */
