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
`test-ext-module.elf` instead of `./bin/native/test-ext-module.elf` (after
entering the `apps/test-ext-module` application directory).


Quick Start
========================

Now checkout RIOT:

    $ git submodule init
    $ git submodule update

Then you can work on any of the applications within the project. Each
subdirectory within the `apps` subdirectory is an application.

Cheatsheet:

    # Change to the application of your choice. e.g.
    cd apps/test-ext-module

    # Build for the native target
    make

    # Run the application
    ./bin/native/test-ext-module.elf

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


ATmega toolchain: Debian
========================

Install required software:

    apt-get install avr-libc gcc-avr avrdude

Verify:

    $ avr-gcc --version
    $ avr-g++ --version

We need avr-g++ for C++, and we need C++ for Arduino programs.

Links:

- <https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#atmel-avr-toolchain>
- <https://github.com/RIOT-OS/RIOT/issues/7109#issuecomment-305089485>


ATmega toolchain: Gentoo
========================

If using Gentoo Linux distribution, this is pretty easy. As described in
<https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#custom-toolchain-via-crossdev-gentoo-only>

Just install crossdev and build the toolchain:

    emerge crossdev
    USE="-openmp -hardened -sanitize -vtv" crossdev -S -t avr

The USE flags are required to build avr-g++

Then `make BOARD=waspmote-pro` should work.


ARM toolchain
=====================

Debian:

    apt-get install gcc-arm-none-eabi

Gentoo:

    crossdev -S -t arm-none-eabi


Boards
=====================

List of boards we have, with CPU and board from RIOT:

- Waspmote v12 (atmega1281/waspmote-pro)
- Waspmote v15 (atmega1281/UNSUPPORTED)
- Arduino Nano (atmega328p/arduino-nano)
- Adafruit Feather M0 (???/feather-m0)

Some boards don't support Arduino, including native, e.g.:

    $ make BOARD=waspmote-pro
    There are unsatisfied feature requirements: arduino


TODO
=====================

Always check open issues and pull requests, what we need may be already there.

- Add support for Waspmote v15
- Fix timer issue with the Waspmote. It seems to sleep forever.
- Add arduino feature to waspmote-pro, and to any other board we need

Caveats:

- To exit the shell type "^D" then "ENTER". To implement an "exit" command we
  would need to modify RIOT.
