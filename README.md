Quick Start
========================

Install requirements (some are optional, see comments). For Debian or Debian
derivatives:

    sudo apt-get update
    sudo apt-get upgrade
    sudo apt install gcc-multilib g++-multilib     # Native port
    sudo apt install gcc-arm-none-eabi             # ARM
    sudo apt install avr-libc gcc-avr avrdude      # AVR
    sudo apt install python3-serial                # To use the terminal (shell)
    sudo apt install openocd                       # Flash to Arduino Zero

Checkout RIOT. It's included as a Git submodule, so you don't need to clone
manually, just initialize and update the submodules:

    git submodule init
    git submodule update

By default programs are compiled for the native port (Linux process). But this
is not very useful, since the native port lacks features required by many
programs, so most often it just doesn't build.

Instead we need to tell which board we want to build the program for. In the
examples below we're using the remote-revb board. For example:

    BOARD=remote-revb make -C apps/wsn-shell

This line will just build the program. To flash it and run the terminal type:

    BOARD=remote-revb make -C apps/wsn-shell all flash term

The programs from the project are in the ``apps`` directory.


Build options
========================

The build can be modified passing some paramters, these may be useful:

    # The board to build for, default is native
    BOARD=remote-revb

    # Port the mote is connected to, default is /dev/ttyACM0
    PORT=/dev/ttyUSB1

    # Baudrate of the USB port, default is 9600
    BAUD=115200

    # Log level: LOG_NONE, LOG_ERROR, LOG_WARNING, LOG_INFO (default), LOG_DEBUG, LOG_ALL
    LOG_LEVEL=LOG_DEBUG

    # Use modules changes behaviour
    USEMODULE=log_color


Programs: wsn-shell
========================

This program will open a shell. It allows for exploration, testing and
configuration of the mote:

    BOARD=remote-revb make -C apps/wsn-shell all flash term

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

    
Programs: wsn-main
========================

This is the main program. It will loop forever:

1. Sample the sensors
2. Send the reading through the network
3. Sleep for a while
4. Repeat

To build this program you must pass the mote's name, like so:

    NODE_ID=remote_cs BOARD=remote-revb make -C apps/wsn-main all flash term

Before entering the sample/send loop it will first try to grab the time from
the network: sending *ping* commands and waiting for a reply with the time.

To avoid this first step, when you're developing and don't have the network
set-up, you can pass the time at build time:

    BASETIME=`date +%s` NODE_ID=remote_cs BOARD=remote-revb make -C apps/wsn-main/ all flash term

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


Optional: direnv
========================

If you don't want to type the node's name again and again, you can use *direnv*
to define it once.

[direnv](https://direnv.net/) allows to export environment variables as you
enter a directory, and unload them as you leave.

Install direnv in Debian or Debian derivatives:

    apt-get install direnv

Create the ``.envrc`` file:

    export NODE_ID=remote_cs

Enable direnv:

    direnv allow

Now you don't need to pass ``NODE_ID`` in the command line:

    BASETIME=`date +%s` BOARD=remote-revb make -C apps/wsn-main/ all flash term


RIOT programs
========================

You can as well try some examples and tests from RIOT:

    BOARD=remote-revb make -C RIOT/examples/default all flash term

Most RIOT tests have a test target, if the test doesn't end with a traceback
then it was a success. For example:

    BOARD=remote-revb make -C RIOT/tests/periph_rtt all flash test


Boards
========================

These are some boards for interest:

- ``remote-revb`` - The main board we use for development.

- ``waspmote-pro`` - The old board used in our 1st generation motes. Some RIOT
  programs may work.

- ``arduino-uno`` and ``arduino-mega2560`` - Official Arduino boards with the
  AVR architecture. However the Uno has very little memory, almost nothing of
  interest will work on it.

- ``arduino-zero`` - Official Arduino board with an ARM MCU.

- ``feather-m0`` - A board from Adafruit with the same MCU as the Arduino Zero (SAMD21).

Board: Arduino Zero
------------------------

To flash programs to the Arduino zero install openocd, see
``https://api.riot-os.org/group__boards__arduino-zero.html``

The board has 2 USB ports: the programming port and the native port.
Use the programming port, the one close to the supply connector.


Documentation
====================

Our API is documented using doxygen. Just run doxygen from the root directory
of the project:

    doxygen

And then open your browser at the proper location,
``file:///[...]/wsn_riot/doc/html/index.html``


Troubleshooting
====================

If, with atmega boards, you get some error flashing like:

    avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x31

Try unplugging and plugging again the USB cable.


TODO
=====================

Always check open issues and pull requests, what we need may be already there.

- Try power saving tests
- Save frames to SD, e.g. sample every 1minute, send every 5minutes.
- Split large frames? Or is this handled by the network stack?
- Move dprintf/dgets to helper module for reuse
- Resolve XXX, FIXME and TODO inline comments
