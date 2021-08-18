/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_sensors
 * @{
 *
 * @file
 * @brief       sensors implementation
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#include <errno.h>
#include "sensors.h"

/**
 * @brief   Keep the head of the device list as global variable
 */
sensor_t *sensors_list = NULL;


int sensors_add(sensor_t *sensor)
{
    if (sensor == NULL)
        return -ENODEV;

    /* prepare new entry */
    sensor->next = NULL;

    /* add to registry */
    if (sensors_list == NULL) {
        sensors_list = sensor;
    } else {
        sensor_t *tmp = sensors_list;
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = sensor;
    }

    return 0;
}
