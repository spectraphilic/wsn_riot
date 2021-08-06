/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_qtpy
 *
 * @{
 * @file
 * @brief       SAUL adaption for the sensor board
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#define ENABLE_DEBUG        1
#include <debug.h>

#include <phydat.h>
#include <saul.h>
#include <saul_reg.h>

#include "qtpy.h"
#include "qtpy_params.h"


qtpy_t qtpy_dev;

static int bme_read_temp(const void *ptr, phydat_t *res) {
    int temp;

    qtpy_t* dev = (qtpy_t*) ptr;

    qtpy_begin(dev);
    qtpy_bme_temp(dev, &temp);
    qtpy_end(dev);

    res->val[0] = temp;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;

    //return -ECANCELED;
}


const saul_driver_t bme_temp_driver = {
    .read = bme_read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};


//static saul_reg_t s0 = { NULL, NULL, "AS7341", &s_driver };
//static saul_reg_t s2 = { NULL, NULL, "ICM", &s_driver };
//static saul_reg_t s3 = { NULL, NULL, "MLX", &s_driver };
//static saul_reg_t s4 = { NULL, NULL, "SHT31", &s_driver };
//static saul_reg_t s5 = { NULL, NULL, "TMP117", &s_driver };
//static saul_reg_t s6 = { NULL, NULL, "VCNL4040", &s_driver };
//static saul_reg_t s7 = { NULL, NULL, "VEML7700", &s_driver };
//static saul_reg_t s8 = { NULL, NULL, "VL53L1", &s_driver };

static saul_reg_t bme_temp_reg = {
    .next = NULL,
    .dev = &qtpy_dev,
    .name = "BME280 temp",
    .driver = &bme_temp_driver
};


void auto_init_qtpy(void)
{
    DEBUG("Auto init sensor board\n");

    qtpy_init(&qtpy_dev, &qtpy_params[0]);

    saul_reg_add(&bme_temp_reg);
}
