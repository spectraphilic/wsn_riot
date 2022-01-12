#ifdef MODULE_DS3231

// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Project
#include <triage.h>
#include <wsn.h>


static const int ISOSTR_LEN = 20;


int cmd_rtc_temp(int argc, char **argv)
{
    // Arguments
    (void)argv;
    if (check_argc(argc, 1) < 0) {
        return -1;
    }

    // Read
    int16_t temp;
    wsn_rtc_temp(&temp);

    // Print
    int t1 = temp / 100;
    int t2 = temp - (t1 * 100);
    printf("Current temperature: %i.%02i°C\n", t1, t2);

    return 0;
}

/* parse ISO date string (YYYY-MM-DDTHH:mm:ss) to struct tm */
static int _tm_from_str(const char *str, struct tm *time)
{
    char tmp[5];

    if (strlen(str) != ISOSTR_LEN - 1) {
        return -1;
    }
    if ((str[4] != '-') || (str[7] != '-') || (str[10] != 'T') ||
        (str[13] != ':') || (str[16] != ':')) {
        return -1;
    }

    memset(time, 0, sizeof(struct tm));

    memcpy(tmp, str, 4);
    tmp[4] = '\0';
    str += 5;
    time->tm_year = atoi(tmp) - 1900;

    memcpy(tmp, str, 2);
    tmp[2] = '\0';
    str += 3;
    time->tm_mon = atoi(tmp) - 1;

    memcpy(tmp, str, 2);
    str += 3;
    time->tm_mday = atoi(tmp);

    memcpy(tmp, str, 2);
    str += 3;
    time->tm_hour = atoi(tmp);

    memcpy(tmp, str, 2);
    str += 3;
    time->tm_min = atoi(tmp);

    memcpy(tmp, str, 2);
    time->tm_sec = atoi(tmp);

    return 0;
}

int cmd_rtc_time(int argc, char **argv)
{
    struct tm time;

    if (argc > 2) {
        printf("Too many arguments\n");
        return -1;
    }

    // Get
    if (argc == 1) {
        wsn_rtc_time_get(&time);
        // Print
        char dstr[ISOSTR_LEN];
        size_t pos = strftime(dstr, ISOSTR_LEN, "%Y-%m-%dT%H:%M:%S", &time);
        dstr[pos] = '\0';
        printf("The current time is: %s\n", dstr);

        return 0;
    }

    // Set
    if (strlen(argv[1]) != (ISOSTR_LEN - 1)) {
        puts("error: input date string has invalid length");
        return -1;
    }

    int res = _tm_from_str(argv[1], &time);
    if (res != 0) {
        puts("error: unable do parse input date string");
        return -1;
    }

    if (wsn_rtc_time_set(&time) == 0) {
        printf("success: time set to %s\n", argv[1]);
    }

    return 0;
}


#endif
