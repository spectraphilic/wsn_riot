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

#include <assert.h>
#include <ztimer.h>

#include <SDI12.h>

#include "qtpy.h"
#include "qtpy_constants.h"
#include "qtpy_params.h"


int qtpy_init(qtpy_t *dev, const qtpy_params_t *params)
{
    assert(dev && params);

    dev->params = *params;

    SDI12 sdi12(params->pin);
    qtpy_begin(dev);

    // TODO Send ack command a!

    return 0;
}

int qtpy_begin(const qtpy_t *dev)
{
    dev->sdi12.begin();
    ztimer_sleep(ZTIMER_MSEC, 500);
    return 0;
}

int qtpy_end(const qtpy_t *dev)
{
    dev->sdi12.end();
    return 0;
}

int qtpy_send_raw(const qtpy_t *dev, const char *cmd, char out[])
{
    // Send command
    dev->sdi12.clearBuffer();
    dev->sdi12.sendCommand(cmd);
    ztimer_sleep(ZTIMER_MSEC, 300);

    // Read answer
    int i = 0;
    while (sdi12.available())
        out[i++] = sdi12.read();
    out[i] = '\0';

    // TODO Skip garbage at the beginning, and \r\n at the end

    return 0;
}

int qtpy_send(const qtpy_t *dev, const char *cmd, char out[])
{
    char raw_command[100];
    snprintf(raw_command, sizeof(raw_command), "%d%s!", dev->params.address, cmd);
    return qtpy_send_raw(dev, raw_command, out);
}

int qtpy_measure(const qtpy_t *dev, unsigned int *ttt, uint8_t number)
{
    size_t size = 5;
    char cmd[size];
    char out[100];

    if (number == 0) {
        qtpy_send(dev, "M", out);
    } else {
        snprintf(cmd, sizeof(cmd), "M%d", number);
        qtpy_send(dev, cmd, out);
    }

    // Parse response: atttn\r\n
    // Not standard, but we support atttnn\r\n as well
    unsigned int a, nn;
    int count = sscanf(out, "%1u%3u%2u", &a, ttt, &nn);
    if (count < 3)
        return -1;

    return nn;
}

int qtpy_data(const qtpy_t *dev, float values[], uint8_t n)
{
    char command[3];
    uint8_t d = 0;
    char out[100];

    int i = 0;
    while (i < n) {
        sprintf(command, "D%hhu", d);
        qtpy_send(dev, command, out);
        if (strlen(out) <= 1) // a\r\n
            return -1;

        char *next = (char*) out + 1;
        while (next[0] != '\0')
            values[i++] = strtod(next, &next);

        d++;
    }

    return i;
}

int qtpy_bme_temp(const qtpy_t *dev, int *temp)
{
    static int n;
    unsigned int ttt;
    float values[30];

    n = qtpy_measure(dev, &ttt, 1);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_SEC, ttt);

        if (qtpy_data(dev, values, n) == n) {
            *temp = values[0];
            //float bme_h = values[1];
            //float bme_p = values[2];
            return 0;
        }
    }

    return -1; // XXX
}
