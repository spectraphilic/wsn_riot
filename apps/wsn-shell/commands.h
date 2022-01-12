#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

int cmd_acc(int argc, char **argv);
int cmd_bme(int argc, char **argv);
int cmd_cat(int argc, char **argv);
int cmd_catx(int argc, char **argv);
int cmd_ds18b20(int argc, char **argv);
int cmd_format(int argc, char **argv);
int cmd_sht(int argc, char **argv);
int cmd_tail(int argc, char **argv);
int cmd_var(int argc, char **argv);
int cmd_rtc_temp(int argc, char **argv);
int cmd_rtc_time(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
