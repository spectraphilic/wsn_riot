App wsn-shell
=============

This test program shows how to use the shell module. By default no commands are available.

The `shell_cmds_default` module provides a list of commands. By default only `reboot` and
`version`, but if other modules are used then extra commands will be available.

For example if, in addition to `shell_cmd_i2c_scan` the `shell_cmd_i2c_scan` module is
used, then the `i2c_scan` command will be available.

You can as well add your own commands, see `main.c`.

This is an example session:

    $ BOARD=lora-e5-wsn make -C apps/wsn-shell all flash term
    [...]
    2022-10-25 13:01:03,393 # i2c_scan 0
    2022-10-25 13:01:03,394 # Scanning I2C device 0...
    2022-10-25 13:01:03,399 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
    2022-10-25 13:01:03,406 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
    2022-10-25 13:01:03,407 # 0x00 R R R R R R R R R R R R R R - -
    2022-10-25 13:01:03,412 # 0x10 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,417 # 0x20 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,424 # 0x30 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,429 # 0x40 - - - - - - - - X - - - - - - -
    2022-10-25 13:01:03,436 # 0x50 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,444 # 0x60 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,446 # 0x70 - - - - - - - - R R R R R R R R

The table below summarizes what works and what doesn't:

| Device    | lora-e5-dev   | waspmote-pro |
| --------- | ------------- | ------------ |
| i2c\_scan | YES[1]        | YES          |
| rtc       | YES[2]        | NO           |
| acc       | NOT AVAIL     | YES          |
| bme280    | NOT TESTED    | NOT TESTED   |
| SHT31     | YES           | NOT TESTED   |
| DS18B20   | NOT TESTED    | NOT TESTED   |

[1] Sometimes it fails
[2] MCU's RTC not supported, tested with external DS3231
