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
    ztimer_sleep(ZTIMER_MSEC, 200);
    return 0;
}

int qtpy_end(qtpy_t *dev)
{
    dev->sdi12.end();
    return 0;
}

int qtpy_send_raw(qtpy_t *dev, const char *cmd)
{
    // Send command
    DEBUG("SDI-12 Send %s\n", cmd);
    dev->sdi12.clearBuffer();
    dev->sdi12.sendCommand(cmd);
    ztimer_sleep(ZTIMER_MSEC, 200);

    // Wait for data
    while (! dev->sdi12.available()) // FIXME Timeout, use sdi12.peekNextDigit ?
        ztimer_sleep(ZTIMER_MSEC, 5);

    // Read answer
    int i = 0;
    while (dev->sdi12.available()) {
        char c = dev->sdi12.read();
        //printf("CHAR %d\n", (int) c);
        dev->out[i++] = c;
    }
    dev->out[i] = '\0';
    DEBUG("SDI-12 Read %s\n", dev->out);

    // TODO Skip garbage at the beginning, and \r\n at the end

    return 0;
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
        sprintf(command, "D%hhu", d);
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
