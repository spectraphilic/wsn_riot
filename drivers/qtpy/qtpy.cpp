/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_qtpy
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Sensor board running the lagopus sketch
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include <debug.h>
#include <ztimer.h>

#include <math.h>
#include <log.h>
#include <phydat.h>
#include "sensors.h"

#include "qtpy.h"
#include "qtpy_constants.h"
#include "qtpy_params.h"


int qtpy_init(qtpy_t *dev, const qtpy_params_t *params)
{
    if (!dev || !params)
        return -EINVAL;

    dev->params = *params;
    qtpy_begin(dev);

    // Send ack command a!
    qtpy_send(dev, "");
    if (dev->out[0] != dev->params.address)
        return -EPROTO;

    return 0;
}

int qtpy_begin(qtpy_t *dev)
{
    dev->sdi12.begin(dev->params.pin);
    ztimer_sleep(ZTIMER_MSEC, 100);
    return 0;
}

int qtpy_end(qtpy_t *dev)
{
    dev->sdi12.end();
    return 0;
}

static int __next(qtpy_t *dev, ztimer_now_t timeout)
{
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);

    while ((ztimer_now(ZTIMER_MSEC) - now) < timeout) {
        if (dev->sdi12.available()) {
            char c = dev->sdi12.read();
            dev->out[dev->idx] = c;
            dev->idx ++;
            return c;
        }
    }

    return -1;
}

int qtpy_send_raw(qtpy_t *dev, const char *cmd)
{
    // Send command
    DEBUG("SDI-12 Send %s\n", cmd);
    dev->sdi12.clearBuffer();
    dev->sdi12.sendCommand(cmd);

    // Read data
    int status = 0;
    dev->idx = 0;

    int c = __next(dev, 200); // Wait up to 200ms to get the first char
    while (c != -1) {
        // TODO Skip garbage at the beginning
        if (status == 0 && c == '\r') {
            status = 1;
        } else if (status == 1 && c == '\n') {
            dev->out[dev->idx - 2] = 0; // Don't keep \r\n
            DEBUG("SDI-12 Read %s\n", dev->out);
            return 0;
        } else {
            status = 0;
        }

        c = __next(dev, 50);
    }

    return -1;
}

int qtpy_send(qtpy_t *dev, const char *cmd)
{
    char raw_command[100];
    snprintf(raw_command, sizeof(raw_command), "%c%s!", dev->params.address, cmd);
    return qtpy_send_raw(dev, raw_command);
}

int qtpy_measure(qtpy_t *dev, unsigned int *ttt, uint8_t number)
{
    char cmd[5];

    if (number == 0) {
        qtpy_send(dev, "M");
    } else {
        snprintf(cmd, sizeof(cmd), "M%d", number);
        qtpy_send(dev, cmd);
    }

    // Parse response: atttn\r\n
    // Not standard, but we support atttnn\r\n as well
    unsigned int a, nn;
    if (sscanf(dev->out, "%1u%3u%2u", &a, ttt, &nn) < 3)
        return -1;

    return nn;
}

int qtpy_data(qtpy_t *dev, float values[], uint8_t n)
{
    char command[3];
    uint8_t d = 0;

    int i = 0;
    while (i < n) {
        sprintf(command, "D%d", d);
        qtpy_send(dev, command);
        if (strlen(dev->out) <= 1) // a\r\n
            return -1;

        char *next = (char*) dev->out + 1;
        while (next[0] != '\0')
            values[i++] = strtod(next, &next);

        d++;
    }

    return i;
}

int qtpy_bme280(qtpy_t *dev, float *temp, float *hum, float *press)
{
    static int n;
    unsigned int ttt;
    float values[20];

    n = qtpy_measure(dev, &ttt, 1);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_SEC, ttt);

        if (qtpy_data(dev, values, n) == n) {
            *temp = values[0];
            *hum = values[1];
            *press = values[2];
            return 0;
        }
    }

    return -1; // XXX
}

int qtpy_sht31(qtpy_t *dev, float *temp, float *hum)
{
    static int n;
    unsigned int ttt;
    float values[20];

    n = qtpy_measure(dev, &ttt, 4);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_SEC, ttt);

        if (qtpy_data(dev, values, n) == n) {
            *temp = values[0];
            *hum = values[1];
            return 0;
        }
    }

    return -1; // XXX
}


/*
 * Register stuff
 */

qtpy_t qtpy_dev;

static int sensor_bme280_read(const void *dev, int state, phydat_t *res) {
    qtpy_t *qtpy = (qtpy_t*) dev;

    static float temp, hum, press;
    switch (state) {
        case 0:
            qtpy_begin(qtpy);
            qtpy_bme280(qtpy, &temp, &hum, &press);
            qtpy_end(qtpy);
            res->val[0] = 210;
            res->unit = UNIT_NONE;
            res->scale = 0;
            return 1;
        case 1:
            res->val[0] = round(temp * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            return 2;
        case 2:
            res->val[0] = round(hum * 100);
            res->unit = UNIT_PERCENT;
            res->scale = -2;
            return 3;
        case 3:
            res->val[0] = round(press);
            res->unit = UNIT_PA;
            res->scale = 0;
            return -1;
    }

    return -1;
}

static int sensor_sht31_read(const void *dev, int state, phydat_t *res) {
    qtpy_t *qtpy = (qtpy_t*) dev;

    static float temp, hum;
    switch (state) {
        case 0:
            qtpy_begin(qtpy);
            qtpy_sht31(qtpy, &temp, &hum);
            qtpy_end(qtpy);
            res->val[0] = 219;
            res->unit = UNIT_NONE;
            res->scale = 0;
            return 1;
        case 1:
            res->val[0] = round(temp * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            return 2;
        case 2:
            res->val[0] = round(hum * 100);
            res->unit = UNIT_PERCENT;
            res->scale = -2;
            return -1;
    }

    return -1;
}

static sensor_t sensor_bme280 = {
    .next = NULL,
    .dev = &qtpy_dev,
    .name = "BME280",
    .read = &sensor_bme280_read,
};

static sensor_t sensor_sht31 = {
    .next = NULL,
    .dev = &qtpy_dev,
    .name = "SHT31",
    .read = &sensor_sht31_read,
};

void qtpy_init_auto(void)
{
    DEBUG("Init sensor board\n");

    switch (qtpy_init(&qtpy_dev, &qtpy_params[0])) {
        case 0:
            break;
        case -EPROTO:
            LOG_ERROR("[QTPY] Protocol error\n");
            return;
        default:
            LOG_ERROR("[QTPY] Unexpected error %d\n");
            return;
    }

    sensors_add(&sensor_bme280);
    sensors_add(&sensor_sht31);
}
