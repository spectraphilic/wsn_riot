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

#include <periph/uart.h>

#ifdef __cplusplus
extern "C" {
#endif

void gps_start(void);
int gps_on(uart_t uart);
void gps_off(void);
void gps_print_data(void);
void gps_send_init_lla(uart_t uart);

#ifdef __cplusplus
}
#endif

#endif /* GPS_H */
/** @} */
