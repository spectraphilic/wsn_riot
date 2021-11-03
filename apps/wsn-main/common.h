#ifndef WSN_COMMON_H
#define WSN_COMMON_H

#include <shell.h>
#include <timex.h>

#ifdef MODULE_ZTIMER_MSEC
    #define ZTIMER ZTIMER_MSEC
    #define TICKS_PER_SEC MS_PER_SEC
#else
    #define ZTIMER ZTIMER_USEC
    #define TICKS_PER_SEC US_PER_SEC
#endif

#ifdef __cplusplus
extern "C" {
#endif

int send_data(const uint8_t *data, size_t size);
void thread_recv_start(void);
void thread_send_start(void);
void thread_sensors_start(void);
extern const shell_command_t shell_commands[];

#ifdef __cplusplus
}
#endif

#endif
