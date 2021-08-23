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
 * @brief   Generic data structure for expressing physical values
 *
 * In addition to the data field, this struct contains further the (physical)
 * unit and the scale factor of the data. The unit is expressed as constant.
 * The scale factor is expressed as power of 10 (10^factor).
 *
 * The combination of signed 32-bit numbers with and the scale factor gives us
 * a very high dynamic range. In a wider sense we are saving the values as
 * fixed floating points.
 *
 * In a traditional (scientific) computational system the obvious choice for the
 * used data type would be to use floats. We are however on heavily resource
 * constrained (even 8-bit) embedded systems, so we use int32_t here.
 *
 * We don't use RIOT's phydat because it uses int16_t for the data, which is
 * not enough for some values. For example for atomospheric pressure expressed
 * in Pa.
 *
 * Also we only have one data point instead of three, because we handle a
 * variable number of data points at a higher level.
 */
typedef struct {
    int32_t value;              /**< the 3 generic dimensions of data */
    uint8_t unit;               /**< the (physical) unit of the data */
    int8_t scale;               /**< the scale factor, 10^*scale* */
} phyval_t;

/**
 * @brief   Read a value (a set of values) from a device
 *
 * Simple sensors, as e.g. a temperature sensor, will return exactly one value
 * together with the values scale and unit. Some sensors might return a tuple
 * or triple of data (e.g. a 3-axis accelerometer). Others will return a number
 * of readings with different units and scale (e.g. BME280 reads temperature,
 * humidity and atmospheric pressure).
 *
 * To support a variable number of arguments, the implementation must behave
 * like a generator, i.e. able to be called multiple times, each one return one
 * value.  Will return 1 while there are readings left, and 0 when done.
 *
 * To do so the funcion must keep an state, using static variables. To make the
 * implementation easier you can use coroutine.h
 *
 * @param[in] dev       device descriptor of the target device
 * @param[out] res      data read from the device
 *
 * @retval  1           if there're readings left
 * @retval  0           if there's nothing left
 * @retval  -ENOTSUP    if the device does not support this operation
 * @retval  -ECANCELED  on other errors
 */
typedef int(*sensor_read_t)(const void *dev, phyval_t *res);

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
