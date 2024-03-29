# Quick Start {#mainpage}

Install system wide requirements (some are optional, see comments). For Debian
or Debian derivatives:

    apt update
    apt upgrade
    apt install doxygen                       # To build the documentation
    apt install python3-serial                # To use the terminal (shell)
    apt install gcc-arm-none-eabi             # ARM
    apt install gcc-multilib g++-multilib     # Native port
    apt install avr-libc gcc-avr avrdude      # AVR
    apt install openocd                       # Flash to Arduino Zero

Checkout RIOT. It's included as a Git submodule, so you don't need to clone
manually, just initialize and update the submodules:

    git submodule init
    git submodule update

Try building our apps (for the `lora-e5-wsn` board by default):

    make -C apps/wsn-shell
    make -C apps/wsn-main

To build, flash, and run the terminal program:

    make -C apps/wsn-shell flash term

Check the `apps` directory for the available programs.

# Boards

In RIOT programs are by default built for the native port (Linux process). But
we have changed this, in `wsn_riot` programs are by default built for the
`lora-e5-wsn` board. Anyway, our apps don't build for the native port, because
they require the arduino feature.

If you wish to try building for a different board, you need to pass the
appropriate build option:

    BOARD=feather-m0 make -C apps/wsn-shell

# Build options

The build can be modified passing some parameters, these may be useful:

    # The board to build for, default is lora-e5-wsn
    BOARD=feather-m0

    # Port the mote is connected to, default is /dev/ttyACM0
    PORT=/dev/ttyUSB1

    # Baudrate of the USB port, default is 9600
    BAUD=115200

    # Log level: LOG_NONE, LOG_ERROR, LOG_WARNING, LOG_INFO (default), LOG_DEBUG, LOG_ALL
    LOG_LEVEL=LOG_DEBUG

    # Wait n seconds before opening the terminal (default is 2)
    TERM_DELAY=1

    # The terminal to use (default is pyterm)
    RIOT_TERMINAL=socat

Modules can be enabled in the command line, for example:

    # Have a shield with the DS3231 RTC (e.g. DS3231 FeatherWing)
    USEMODULE=ds3231

    # Have a shield with a SX1276 module (e.g the RFM95W FeatherWing)
    USEMODULE=sx1276

    # For wsn-main, will print process information on boot
    USEMODULE=ps

    # Type "s" to start
    USEMODULE=test_utils_interactive_sync

Examples with CFLAGS:

    # Don't print the boot message "main(): This is RIOT! ..."
    CFLAGS="-DCONFIG_SKIP_BOOT_MSG"

Curently our datalogger is based on the LoRa-E5 mini board. So you should build with this
configuration:

    USEMODULE="ds3231 sx1276" make -C apps/wsn-shell flash term

# Programs: wsn-shell

This program will open a shell. It allows for exploration, testing and
configuration of the mote:

    make -C apps/wsn-shell flash term

In the shell you can type a number of commands. For example, ``help``` lists
the commands available:

    > help
    2021-08-24 11:28:44,011 # help
    2021-08-24 11:28:44,013 # Command              Description
    2021-08-24 11:28:44,015 # ---------------------------------------
    2021-08-24 11:28:44,017 # cat                  print contents of given filename
    2021-08-24 11:28:44,021 # sdi                  send SDI-12 commands
    2021-08-24 11:28:44,027 # tail                 print the last N lines of given filename
    2021-08-24 11:28:44,036 # var                  var [NAME [VALUE]] - type 'var' to list the variable names
    2021-08-24 11:28:44,039 # reboot               Reboot the node
    2021-08-24 11:28:44,043 # version              Prints current RIOT_VERSION
    2021-08-24 11:28:44,048 # pm                   interact with layered PM subsystem
    2021-08-24 11:28:44,051 # random_init          initializes the PRNG
    2021-08-24 11:28:44,056 # random_get           returns 32 bit of pseudo randomness
    2021-08-24 11:28:44,060 # ifconfig             Configure network interfaces
    2021-08-24 11:28:44,066 # saul                 interact with sensors and actuators using SAUL
    2021-08-24 11:28:44,073 # vfs                  virtual file system operations
    2021-08-24 11:28:44,075 # ls                   list files
    2021-08-24 11:28:44,077 # i2c_scan             Performs an I2C bus scan

The ``ls`` commands lists files in the SD card:

    > ls /
    2021-08-24 11:35:09,405 # ls /
    2021-08-24 11:35:09,408 # LOG2.TXT
    2021-08-24 11:35:09,409 # total 1 files

The ``i2c_scan`` command explores the I2C bus:

    > i2c_scan 0
    2021-08-24 11:36:00,384 # i2c_scan 0
    2021-08-24 11:36:00,386 # Scanning I2C device 0...
    2021-08-24 11:36:00,389 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
    2021-08-24 11:36:00,391 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
    2021-08-24 11:36:00,393 # 0x00 R R R R R R R R R R R R R R - -
    2021-08-24 11:36:00,394 # 0x10 - - - - - - - - - - - - - - - -
    2021-08-24 11:36:00,398 # 0x20 - - - - - - - - - - - - - - - -
    2021-08-24 11:36:00,402 # 0x30 - - - - - - - - - - - - - - - -
    2021-08-24 11:36:00,407 # 0x40 - - - - - - - - - - - - - - - -
    2021-08-24 11:36:00,411 # 0x50 - - - - - - - - - - - - - - - -
    2021-08-24 11:36:00,416 # 0x60 - - - - - - - - - X - - - - - -
    2021-08-24 11:36:00,421 # 0x70 - - - - - - - - R R R R R R R R

The ``sdi`` command sends any SDI-12 command:

    > sdi ?!
    2021-08-24 11:58:54,078 # sdi ?!
    2021-08-24 11:58:54,248 # => 5
    > sdi 5I!
    2021-08-24 11:58:58,422 # sdi 5I!
    2021-08-24 11:58:58,761 # => 513UOSLOGEOLAGOPU000
    > sdi 5M1!
    2021-08-24 11:59:20,185 # sdi 5M1!
    2021-08-24 11:59:20,406 # => 50013
    > sdi 5D0!
    2021-08-24 11:59:23,185 # sdi 5D0!
    2021-08-24 11:59:23,532 # => 5+21.01+68.06+892.26


# Programs: wsn-main

This is the main program. It will loop forever:

1. Sample the sensors
2. Send the reading through the network
3. Sleep for a while
4. Repeat

To build this program you must pass the mote's name, like so:

    NODE_ID=cas1 make -C apps/wsn-main flash term

Before entering the sample/send loop it will first try to grab the time from
the network: sending *ping* commands and waiting for a reply with the time.

To avoid this first step, when you're developing and don't have the network
set-up, you can pass the time at build time:

    BASETIME=`date +%s` NODE_ID=cas1 make -C apps/wsn-main/ flash term

This is an excerpt of a loop:

    2021-08-24 12:13:17,392 # 260.716 INFO    Loop=8
    2021-08-24 12:13:17,392 # QT Py:
    [...]
    2021-08-24 12:13:29,944 # CBOR = 9f001a6124c61b011b004b12002e154019026972656d6f74655f6373030818dc181e182e183c184d19027b19019b185c186c189018b118d219084c191a9f1a00015c6718d31908c31908a118db1908a0191ba518d41908a518de0118f219024e18df1974a41a000154da19144018d50f19089e0000000000000000000000000000ff
    2021-08-24 12:13:29,946 # gnrc_pktbuf_add(...) size = 130
    2021-08-24 12:13:29,947 # 273.264 INFO    Loop=8 DONE

You can verify the data, with the ``riot.py`` program from the ``wsn_pi`` project:

    (venv) $ python riot.py 9f001a6124c61b011b004b12002e154019026972656d6f74655f6373030818dc181e182e183c184d19027b19019b185c186c189018b118d219084c191a9f1a00015c6718d31908c31908a118db1908a0191ba518d41908a518de0118f219024e18df1974a41a000154da19144018d50f19089e0000000000000000000000000000ff
    Frame size = 130
    {...}


# Optional: direnv

If you don't want to type the node's name again and again, you can use *direnv*
to define it once.

[direnv](https://direnv.net/) allows to export environment variables as you
enter a directory, and unload them as you leave.

Install direnv in Debian or Debian derivatives:

    apt-get install direnv

Create the ``.envrc`` file:

    export NODE_ID=cas1

Enable direnv:

    direnv allow

Now you don't need to pass ``NODE_ID`` in the command line:

    BASETIME=`date +%s` make -C apps/wsn-main/ flash term


# RIOT programs

You can as well try some examples and tests from RIOT:

    BOARD=lora-e5-dev make -C RIOT/examples/default flash term

Most RIOT tests have a test target, if the test doesn't end with a traceback
then it was a success. For example:

    BOARD=lora-e5-dev make -C RIOT/tests/periph_rtt flash test


# Documentation

Our API is documented using doxygen. Just run doxygen from the root directory
of the project:

    doxygen

And then open your browser at the proper location,
``file:///[...]/wsn_riot/doc/html/index.html``


# TODO

Always check open issues and pull requests, what we need may be already there.

- Try power saving tests
- Save frames to SD, e.g. sample every 1minute, send every 5minutes.
- Split large frames? Or is this handled by the network stack?
- Move dprintf/dgets to helper module for reuse
- Resolve XXX, FIXME and TODO inline comments
