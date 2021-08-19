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


static int qtpy_measure_data(qtpy_t *dev, uint8_t number, float values[])
{
    static int n;
    unsigned int ttt;

    n = qtpy_measure(dev, &ttt, number);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_SEC, ttt);

        if (qtpy_data(dev, values, n) == n)
            return 0;
    }

    return -1; // XXX
}


/*
 * Register stuff
 */

qtpy_t qtpy_dev;

static int sensor_qtpy_read(const void *ptr, phydat_t *res) {
    qtpy_t *dev = (qtpy_t*) ptr;

    static int state = 0;
    static float values[20];

    switch (state) {
        // AS7341
        case 0:
            qtpy_begin(dev);
            qtpy_measure_data(dev, 0, values); // aM! : f1, f2, f3, f4, f5, f6, f7, f8, clear, nir
            res->val[0] = 220;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 1:
            res->val[0] = (uint16_t) values[0]; // f1
            res->unit = UNIT_UNDEF;
            res->scale = 0;
            break;
        case 2:
            res->val[0] = (uint16_t) values[1]; // f2
            break;
        case 3:
            res->val[0] = (uint16_t) values[2]; // f3
            break;
        case 4:
            res->val[0] = (uint16_t) values[3]; // f4
            break;
        case 5:
            res->val[0] = (uint16_t) values[4]; // f5
            break;
        case 6:
            res->val[0] = (uint16_t) values[5]; // f6
            break;
        case 7:
            res->val[0] = (uint16_t) values[6]; // f7
            break;
        case 8:
            res->val[0] = (uint16_t) values[7]; // f8
            break;
        case 9:
            res->val[0] = (uint16_t) values[8]; // clear
            break;
        case 10:
            res->val[0] = (uint16_t) values[9]; // nir
            break;

        // BME280
        case 11:
            qtpy_measure_data(dev, 1, values); // aM1! : temp, humidity, pressure
            res->val[0] = 210;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 12:
            res->val[0] = round(values[0] * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            break;
        case 13:
            res->val[0] = round(values[1] * 100);
            res->unit = UNIT_PERCENT;
            res->scale = -2;
            break;
        case 14:
            res->val[0] = round(values[2]);
            res->unit = UNIT_PA;
            res->scale = 0;
            break;

        // ICM20X TODO Doesn't work in the lagopus shield
//      case :
//          qtpy_measure_data(dev, 2, values); // aM2! : temp, acc(xyz), mag(xyz), gyro(xyz)
//          res->val[0] = 221;
//          res->unit = UNIT_NONE;
//          res->scale = 0;
//          break;

        // MLX90614
        case 15:
            qtpy_measure_data(dev, 3, values); // aM3! : object temperature, ambient temperature
            res->val[0] = 211;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 16:
            res->val[0] = round(values[0] * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            break;
        case 17:
            res->val[0] = round(values[1] * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            break;

        // SHT31
        case 18:
            qtpy_measure_data(dev, 4, values); // aM4! : temperature, humidity
            res->val[0] = 219;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 19:
            res->val[0] = round(values[0] * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            break;
        case 20:
            res->val[0] = round(values[1] * 100);
            res->unit = UNIT_PERCENT;
            res->scale = -2;
            break;

        // TMP1XX
        case 21:
            qtpy_measure_data(dev, 5, values); // aM5! : temperature
            res->val[0] = 212;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 22:
            res->val[0] = round(values[0] * 100);
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            break;

        // VCNL4040
        case 23:
            qtpy_measure_data(dev, 6, values); // aM6! : prox, lux, white
            res->val[0] = 222;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 24:
            res->val[0] = (uint16_t) values[0]; // prox
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;
        case 25:
            res->val[0] = (uint16_t) values[1]; // lux
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;
        case 26:
            res->val[0] = (uint16_t) values[2]; // white
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;

        // VEML7700
        case 27:
            qtpy_measure_data(dev, 7, values); // aM7! : lux, white, als
            res->val[0] = 223;
            res->unit = UNIT_NONE;
            res->scale = 0;
            break;
        case 28:
            res->val[0] = (uint16_t) values[0]; // lux
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;
        case 29:
            res->val[0] = (uint16_t) values[1]; // white
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;
        case 30:
            res->val[0] = (uint16_t) values[2]; // als
            res->unit = UNIT_UNDEF; // XXX
            res->scale = 0;
            break;

        // VL53L1X TODO
//      case 31:
//          qtpy_measure_data(dev, 8, values); // aM8! : n, distance, ..., distance
//          res->val[0] = 213;
//          res->unit = UNIT_NONE;
//          res->scale = 0;
//          break;

        default:
            state = 0;
            qtpy_end(dev);
            return 0;
    }

    state++;
    return 1; // Continue
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

    sensors_add(&sensor_qtpy);
}
