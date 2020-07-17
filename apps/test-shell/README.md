This test program shows how to use the shell module. By default no commands
are available.

The `shell_commands` module provides a list of commands. By default only
`reboot` and `version`, but if other modules are used then extra commands will
be available.

For example if, in addition to `shell_commands` the `i2c_scan` module is used,
then the `i2c_scan` command will be available.

You can as well add your own commands, see `main.c`.

This is an example session:

``` 
$ make all BOARD=remote-revb flash term
[...]
Welcome to pyterm!
Type '/exit' to exit.
2020-07-16 13:03:37,306 # main(): This is RIOT! (Version: 2020.04)
2020-07-16 13:03:37,307 # app: test-shell
2020-07-16 13:03:37,309 # Board=remote-revb MCU=cc2538
> i2c_scan 0
2020-07-16 13:04:38,397 #  i2c_scan 0
2020-07-16 13:04:38,399 # Scanning I2C device 0...
2020-07-16 13:04:38,403 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
2020-07-16 13:04:38,405 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
2020-07-16 13:04:38,407 # 0x00 R R R R R R R R R R R R R R - -
2020-07-16 13:04:38,409 # 0x10 - - - - - - - - - - - - - - - -
2020-07-16 13:04:38,412 # 0x20 - - - - - - - - - - - - - - - -
2020-07-16 13:04:38,413 # 0x30 - - - - - - - - - - - - - - - -
2020-07-16 13:04:38,414 # 0x40 - - - - - - - - - - - - - - - -
2020-07-16 13:04:38,414 # 0x50 - - - - - - - - - - - - - - - -
2020-07-16 13:04:38,415 # 0x60 - - - - - - - - - X - - - - - -
2020-07-16 13:04:38,416 # 0x70 - - - - - - - - R R R R R R R R
> /exit
2020-07-16 13:04:45,468 # Exiting Pyterm
``` 

The table below summarizes what works and what doesn't:

                remote-revb    waspmote-pro
    i2c_scan            YES             YES
    rtc                 YES              NO
