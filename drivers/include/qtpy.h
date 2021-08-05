/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_qtpy Sensor board running the lagopus sketch
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the sensor board running the lagopus sketch
 *
 * @{
 *
 * @file
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef QTPY_H
#define QTPY_H

#include <stdint.h>

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    int8_t pin;
    int8_t address;
} qtpy_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    qtpy_params_t params;
    //SDI12 sdi12;
} qtpy_t;

/**
 * @brief   Initialize the given device
 *
 * @param[out]  dev     Device descriptor of the driver
 * @param[in]   params  Initialization parameters
 *
 * @retval      0       Success
 */
int qtpy_init(qtpy_t *dev, const qtpy_params_t *params);

/**
 * @brief   Begin the connection
 */
int qtpy_begin(const qtpy_t *dev);

/**
 * @brief   End the connection
 */
int qtpy_end(const qtpy_t *dev);

/**
 * @brief   Send a command and read the answer
 */
int qtpy_send_raw(const qtpy_t *dev, const char *cmd, char out[]);

/**
 * @brief   Send a command to the configured address, read the answer
 *
 * The given command must not include the address nor the ending exclamation
 * mark. These will be added by the function: e.g. "I" will become "5I!" (if 5
 * is the sensor address).
 */
int qtpy_send(const qtpy_t *dev, const char *cmd, char out[]);


// TODO Document
int qtpy_measure(const qtpy_t *dev, unsigned int *ttt, uint8_t number);
int qtpy_data(const qtpy_t *dev, float values[], uint8_t n);
int qtpy_bme_temp(const qtpy_t *dev, int *temp);
void auto_init_qtpy(void);

#ifdef __cplusplus
}
#endif

#endif /* QTPY_H */
/** @} */
