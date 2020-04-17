Prerequisites
========================

Install pyserial system wide. This is required for pyterm to work; pyterm is
the terminal program included in RIOT. The terminal is the program you use
to interact with the application flashed to the board.

With Debian or Ubuntu:

    apt-get install python-serial


Optional: direnv
------------------------

This is optional, but recommended: [direnv](https://direnv.net/) allows to
export environment variables as you enter a directory, and unload them as you
leave.

Install direnv in Debian or Ubuntu:

    apt-get install direnv

Enable direnv:

    direnv allow

This project includes a `.envrc` file in the root directory. It adds
`./bin/native/` to the `PATH`. For example, this way you can just type
`wsn-test.elf` instead of `./bin/native/wsn-test.elf` (after entering the
`apps/test` application directory).


Quick Start
========================

Now checkout RIOT:

    $ git submodule init
    $ git submodule update

Then you can work on any of the applications within the project. Each
subdirectory within the `apps` subdirectory is an application.

Cheatsheet:

    # Change to the application of your choice. e.g.
    cd apps/test

    # Build for the native target
    make

    # Run the application
    ./bin/native/wsn-test.elf

    # Build for the waspmote board
    make BOARD=waspmote-pro

    # Flash the application to the board
    make flash BOARD=waspmote-pro

    # Flash the application to the board connected to the given port
    make flash BOARD=waspmote-pro PORT=/dev/ttyUSB0

    # Run the terminal
    make term BOARD=waspmote-pro PORT=/dev/ttyUSB0

You can pass options to the make command, other than BOARD:

    # Port the mote is connected to, default is /dev/ttyACM0
    PORT=/dev/ttyUSB0

    # Baudrate of the USB port, default is 9600
    BAUD=115200


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
