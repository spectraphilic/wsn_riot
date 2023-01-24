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

//#define ENABLE_DEBUG 1
#include <debug.h>
#include <math.h>

// Riot
#include <log.h>
#include <timex.h>
#include <tiny_strerror.h>
#include <ztimer.h>

// Project
#include <coroutine.h>
#include <sensors.h>

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

static int qtpy_measure(qtpy_t *dev, unsigned int *ttt, uint8_t number)
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

static int qtpy_data(qtpy_t *dev, float values[], uint8_t n)
{
    char command[5];
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


static int qtpy_measure_data(qtpy_t *dev, uint8_t number, float values[])
{
    static int n;
    unsigned int ttt;

    n = qtpy_measure(dev, &ttt, number);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_MSEC, ttt * MS_PER_SEC);

        if (qtpy_data(dev, values, n) == n)
            return n;
    }

    return -1;
}


/*
 * Register stuff
 */

qtpy_t qtpy_dev;

static void fill_data(phyval_t *res, int32_t value, uint8_t unit, int8_t scale)
{
    res->value = value;
    res->unit = unit;
    res->scale = scale;
}

static int sensor_qtpy_read(const void *ptr, phyval_t *res)
{
    static float values[20];
    static int n = 0;
    static int i;

    qtpy_t *dev = (qtpy_t*) ptr;

    scrBegin;

    // TODO Replace UNIT_UNDEF by a proper unit, if it exists in phydat.h

    // AS7341
    qtpy_begin(dev);
    if (qtpy_measure_data(dev, 0, values) > 0) { // aM! : f1, f2, f3, f4, f5, f6, f7, f8, clear, nir
        fill_data(res, 220, UNIT_NONE, 0);
        scrReturn(1);
        for (i = 0; i < 10; i++) {
            fill_data(res, (uint16_t) values[i], UNIT_UNDEF, 0);
            scrReturn(1);
        }
    }

    // BME280
    if (qtpy_measure_data(dev, 1, values) > 0) { // aM1! : temp, humidity, pressure
        fill_data(res, 210, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, round(values[0] * 100), UNIT_TEMP_C, -2);
        scrReturn(1);
        fill_data(res, round(values[1] * 100), UNIT_PERCENT, -2);
        scrReturn(1);
        fill_data(res, round(values[2] * 100), UNIT_PA, 0); // BME280 sends hPa, x100 to transofrm to Pa
        scrReturn(1);
    }

    // ICM20X Doesn't work in the lagopus shield
//  if (qtpy_measure_data(dev, 2, values) > 0) { // aM2! : temp, acc(xyz), mag(xyz), gyro(xyz)
//      fill_data(res, 221, UNIT_NONE, 0);
//      scrReturn(1);
//      for (i = 0; i < 10; i++) {
//          fill_data(res, round(values[0] * 100), UNIT_UNDEF, -2);
//          scrReturn(1);
//      }
//  }

    // MLX90614
    if (qtpy_measure_data(dev, 3, values) > 0) { // aM3! : object temperature, ambient temperature
        fill_data(res, 211, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, round(values[0] * 100), UNIT_TEMP_C, -2);
        scrReturn(1);
        fill_data(res, round(values[1] * 100), UNIT_TEMP_C, -2);
        scrReturn(1);
    }

    // SHT31
    if (qtpy_measure_data(dev, 4, values) > 0) { // aM4! : temperature, humidity
        fill_data(res, 219, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, round(values[0] * 100), UNIT_TEMP_C, -2);
        scrReturn(1);
        fill_data(res, round(values[1] * 100), UNIT_PERCENT, -2);
        scrReturn(1);
    }

    // TMP1XX
    if (qtpy_measure_data(dev, 5, values) > 0) { // aM5! : temperature
        fill_data(res, 212, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, round(values[0] * 100), UNIT_TEMP_C, -2);
        scrReturn(1);
    }

    // VCNL4040
    if (qtpy_measure_data(dev, 6, values) > 0) { // aM6! : prox, lux, white
        fill_data(res, 222, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, (uint16_t) values[0], UNIT_UNDEF, 0); // prox
        scrReturn(1);
        fill_data(res, (uint16_t) values[1], UNIT_UNDEF, 0); // lux
        scrReturn(1);
        fill_data(res, (uint16_t) values[2], UNIT_UNDEF, 0); // white
        scrReturn(1);
    }

    // VEML7700
    if (qtpy_measure_data(dev, 7, values) > 0) { // aM7! : lux, white, als
        fill_data(res, 223, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, round(values[0] * 100), UNIT_UNDEF, -2); // lux
        scrReturn(1);
        fill_data(res, round(values[1] * 100), UNIT_UNDEF, -2); // white
        scrReturn(1);
        fill_data(res, (uint16_t) values[2], UNIT_UNDEF, 0); // als
        scrReturn(1);
    }

    // VL53L1X
    n = qtpy_measure_data(dev, 8, values); // aM8! : n, distance, ..., distance
    if (n > 0) {
        fill_data(res, 213, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, n, UNIT_NONE, 0);
        scrReturn(1);
        fill_data(res, (unsigned int) values[0], UNIT_UNDEF, 0);
        scrReturn(1);
        for (i = 1; i < n; i++) {
            fill_data(res, (unsigned int) values[i] - (unsigned int) values[i-1], UNIT_UNDEF, 0);
            scrReturn(1);
        }
    }

    qtpy_end(dev);

    scrFinish(0);
}

static sensor_t sensor_qtpy = {
    .next = NULL,
    .dev = &qtpy_dev,
    .name = "QT Py",
    .read = &sensor_qtpy_read,
};

void qtpy_init_auto(void)
{
    DEBUG("Init sensor board\n");

    int error = qtpy_init(&qtpy_dev, &qtpy_params[0]);
    switch (error) {
        case 0:
            break;
        case -EPROTO:
            LOG_ERROR("[QTPY] Protocol error");
            return;
        default:
            LOG_ERROR("[QTPY] Unexpected error %s", tiny_strerror(error));
            return;
    }

    sensors_add(&sensor_qtpy);
}
