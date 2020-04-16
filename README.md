Quick Start
========================

Install pyserial. With Debian:

    # apt-get install python-serial

This is required by the default RIOT terminal (pyterm). The terminal is the
user interface you use to interact with the boards, through USB.

Now checkout RIOT:

    $ git submodule init
    $ git submodule update

Then you can work on any of the programs within the project. Each subdirectory
(except RIOT) is a program.

Cheatsheet:

    # Change to the program of your choice. e.g.
    cd test

    # Build for the native target
    make

    # Run the program
    ./bin/native/wsn-test.elf

    # Build for the waspmote board
    make BOARD=waspmote-pro

    # Flash the application to the board
    make flash BOARD=waspmote-pro

    # Flash the application to the board connected to the given port
    make flash BOARD=waspmote-pro PORT=/dev/ttyUSB0

    # Run the terminal
    make term BOARD=waspmote-pro PORT=/dev/ttyUSB0

You can pass options to the make program, other than BOARD:

    # Port the mote is connected to, default is /dev/ttyACM0
    PORT=/dev/ttyUSB0

    # Baudrate of the USB port, default is 9600
    BAUD=115200

Notes:

- To run the terminal (make term) you need to install pyserial. With Debian:
  apt-get install python-serial


Optional: direnv
========================

This is optional, but recommended: [direnv](https://direnv.net/) allows to
export environment variables as you enter the directory, and unload them as you
leave.

This is very handy, and this project includes a `.envrc` file so, after
building the native target, you can just type `uio-wsn.elf` instead of
`./bin/native/uio-wsn.elf`.

If using Debian or Ubuntu just type `apt-get install direnv` to install it.
Then within the root of the project type `direnv allow` (it will prompt you
to do so anyway).


ATmega toolchain: Gentoo
========================

If using Gentoo Linux distribution, this is pretty easy. As described in
<https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#custom-toolchain-via-crossdev-gentoo-only>

Just install crossdev and build the toolchain:

    emerge crossdev
    crossdev -S -t avr

Then `make BOARD=waspmote-pro` should work.


ATmega toolchain: Debian
========================

See <https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#atmel-avr-toolchain>

Installing gcc-avr *may* be enough.

TODO
