
#define ENABLE_DEBUG        1
#include <debug.h>

#include <phydat.h>
#include <saul_reg.h>
#include <SDI12.h>
#include <ztimer.h>

#include "wsn.h"

#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 13
#endif

SDI12 sdi12(SDI12_DATA_PIN);

int wsn_sdi_command(const char* cmd, char out[])
{
    // Start
    sdi12.begin();
    delay(500);

    // Send command
    sdi12.clearBuffer();
    sdi12.sendCommand(cmd);
    delay(300);

    // Read answer
    int i = 0;
    while (sdi12.available())
        out[i++] = sdi12.read();
    out[i] = '\0';

    // TODO Skip garbage at the beginning, and \r\n at the end

    // Done
    sdi12.end();
    return 0;
}

static int send_command(uint8_t address, const char* cmd, char out[])
{
    // Format arguments
    char raw_command[100];
    snprintf(raw_command, sizeof(raw_command), "%d%s!", address, cmd);
    return wsn_sdi_command(raw_command, out);
}

static int measure(unsigned int *ttt, uint8_t address, uint8_t number)
{
    size_t size = 5;
    char cmd[size];
    char out[100];

    if (number == 0) {
        send_command(address, "M", out);
    } else {
        snprintf(cmd, sizeof(cmd), "M%d", number);
        send_command(address, cmd, out);
    }

    // Parse response: atttn\r\n
    // Not standard, but we support atttnn\r\n as well
    unsigned int a, nn;
    int count = sscanf(out, "%1u%3u%2u", &a, ttt, &nn);
    if (count < 3)
        return -1;

    return nn;
}

static int data(float values[], uint8_t address, uint8_t n)
{
    char command[3];
    uint8_t d = 0;
    char out[100];

    int i = 0;
    while (i < n) {
        sprintf(command, "D%hhu", d);
        send_command(address, command, out);
        if (strlen(out) <= 1) // a\r\n
            return -1;

        char *next = (char*) out + 1;
        while (next[0] != '\0')
            values[i++] = strtod(next, &next);

        d++;
    }

    return i;
}


static int bme_read_temp(const void *dev, phydat_t *res) {
    (void)dev; // XXX I think we should have a driver for SDI-12

    static int n;
    unsigned int ttt;
    const int address = 5;
    float values[30];

    n = measure(&ttt, address, 1);
    if (n > 0) {
        if (ttt > 0)
            ztimer_sleep(ZTIMER_SEC, ttt);

        if (data(values, address, n) == n) {
            float bme_t = values[0];
            //float bme_h = values[1];
            //float bme_p = values[2];

            res->val[0] = bme_t;
            res->unit = UNIT_TEMP_C;
            res->scale = -2;
            return 1;
        }
    }

    return -ECANCELED;
}


static const saul_driver_t s1_driver = {bme_read_temp, saul_notsup, SAUL_SENSE_TEMP};

//static saul_reg_t s0 = { NULL, NULL, "AS7341", &s_driver };
static saul_reg_t s1 = { NULL, NULL, "BME280", &s1_driver };
//static saul_reg_t s2 = { NULL, NULL, "ICM", &s_driver };
//static saul_reg_t s3 = { NULL, NULL, "MLX", &s_driver };
//static saul_reg_t s4 = { NULL, NULL, "SHT31", &s_driver };
//static saul_reg_t s5 = { NULL, NULL, "TMP117", &s_driver };
//static saul_reg_t s6 = { NULL, NULL, "VCNL4040", &s_driver };
//static saul_reg_t s7 = { NULL, NULL, "VEML7700", &s_driver };
//static saul_reg_t s8 = { NULL, NULL, "VL53L1", &s_driver };


int wsn_sensors_init(void)
{
    DEBUG("WSN Sensors init\n");

    saul_reg_add(&s1);

    return 0;
}
