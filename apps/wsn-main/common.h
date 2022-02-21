#ifndef WSN_COMMON_H
#define WSN_COMMON_H

#include <shell.h>
#include <timex.h>

#ifdef __cplusplus
extern "C" {
#endif

int send_frame(void);
kernel_pid_t thread_recv_start(void);
void thread_send_start(void);
kernel_pid_t thread_sensors_start(void);
extern const shell_command_t shell_commands[];

int _ps_handler(int argc, char **argv); // XXX For debugging purposes

#ifdef __cplusplus
}
#endif

#endif
