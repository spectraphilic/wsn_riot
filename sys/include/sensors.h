/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_sensors sensors
 * @ingroup     sys
 * @brief       Sensors registry
 *
 * @{
 *
 * @file
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <phydat.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Read a value (a set of values) from a device
 *
 * Simple sensors, as e.g. a temperature sensor, will return exactly one value
 * together with the values scale and unit. Some sensors might return a tuple
 * or triple of data (e.g. a 3-axis accelerometer).
 *
 * Actuators can chose to either just return -ENOTSUP or to return their current
 * set value (e.g. useful for reading back the current state of a switch)
 *
 * @param[in] dev       device descriptor of the target device
 * @param[out] res      data read from the device
 *
 * @return  number of values written into to result data structure [1-3]
 * @return  -ENOTSUP if the device does not support this operation
 * @return  -ECANCELED on other errors
 */
typedef int(*sensor_read_t)(const void *dev, phydat_t *res);

/**
 * @brief   Sensor registry entry
 */
typedef struct sensor {
    struct sensor *next;            /**< pointer to the next device */
    void *dev;                      /**< pointer to the device descriptor */
    const char *name;               /**< string identifier for the device */
    /* Driver TODO Should be a pointer to a different (const) struct */
    sensor_read_t read;             /**< read function pointer */
} sensor_t;

/**
 * @brief   Export the sensors registry as global variable
 */
extern sensor_t *sensors_list;

/**
 * @brief   Register a device with the sensors registry
 *
 * @note    Make sure the registry entry the @p dev pointer is pointing to
 *          resides in some persistent memory location and not on some position
 *          on the stack where it will be overwritten...
 *
 * @param[in] dev       pointer to a pre-populated registry entry
 *
 * @return      0 on success
 * @return      -ENODEV on invalid entry given
 */
int sensors_add(sensor_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SENSORS_H */
/** @} */
