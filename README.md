Quick Start
========================

Install requirements, for Debian or Debian derivatives:

    sudo apt-get update
    sudo apt-get upgrade
    sudo apt install gcc-multilib g++-multilib     # Native port
    sudo apt install avr-libc gcc-avr avrdude      # AVR
    sudo apt install gcc-arm-none-eabi             # ARM
    sudo apt install python3-serial                # To use the terminal

Checkout RIOT. It's included as a Git submodule, so you don't need to clone
manually, just initialize and update the submodules:

    $ git submodule init
    $ git submodule update

Try some examples from RIOT. There are a number of examples and tests within
RIOT:

    cd RIOT/
    make -C examples/default             # Build 'examples/default' for the native target
    make -C examples/default term        # Run the program and open a terminal
    make -C examples/default all term    # Do both in 1 step: build and run in a terminal

You can try the programs with a board as well, then there's an extra step,
flashing:

    # Build, flash and open RIOT app. in a terminal.
    # Example here with the Zolertia Re-mote Rev B board
    BOARD=remote-revb make -C examples/default all flash term

Most RIOT tests have a test target, if the test doesn't end with a traceback
then it was a success. For example:

    BOARD=remote-revb make -C tests/periph_rtt all flash test

Then you can work on any of the applications within the project. Each
subdirectory within the `apps` subdirectory is an application.

Cheatsheet (from the project's root directory):

    # Build for the native target
    make -C apps/wsn-main

    # Run the application
    make -C apps/wsn-main term

    # Build for the waspmote-pro board
    BOARD=waspmote-pro make -C apps/wsn-main

    # Flash the application to the waspmote-pro board
    BOARD=waspmote-pro make -C apps/wsn-main flash

    # Run the terminal
    BOARD=waspmote-pro BOOTLOADER_BAUD=115200 make -C apps/wsn-main term

You can pass options to the make command, other than BOARD:

    # Port the mote is connected to, default is /dev/ttyACM0
    PORT=/dev/ttyUSB1

    # Baudrate of the USB port, default is 9600
    BAUD=115200

    # Log level: LOG_NONE, LOG_ERROR, LOG_WARNING, LOG_INFO (default), LOG_DEBUG, LOG_ALL
    LOG_LEVEL=LOG_DEBUG

    # Use modules changes behaviour
    USEMODULE=log_color

If you get some error flashing like:

    avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x31

Try unplugging and plugging again the USB cable.


RIOT
========================

RIOT is included in this repository, as a submodule:

    $ git submodule init
    $ git submodule update

Now you need to install a cross toolchain for every target architecture.

Toolchain: Native
========================

This allows running a RIOT program as a native Linux process, useful for rapid
development.

Debian:

    $ sudo apt install gcc-multilib g++-multilib

Test:

    $ cd RIOT/examples/default/
    $ make


Toolchain: AVR
========================

Debian:

    $ sudo apt install avr-libc gcc-avr avrdude

Gentoo (the USE flags are required to build avr-g++):

    $ sudo emerge crossdev
    $ USE="-openmp -hardened -sanitize -vtv" crossdev -S -t avr

Verify:

    $ avr-gcc --version
    $ avr-g++ --version

Test:

    $ cd RIOT/examples/default/
    $ make BOARD=waspmote-pro

Links:

- <https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#atmel-avr-toolchain>
- <https://github.com/RIOT-OS/RIOT/issues/7109#issuecomment-305089485>


Toolchain: ARM
========================

Debian:

    $ sudo apt install gcc-arm-none-eabi

Verify:

    $ arm-none-eabi-gcc --version
    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)
    [...]

Test:

    $ cd RIOT/examples/default/


ARM from upstream
------------------------

Alternatively you can install the ARM toolchain from upstream. This works
regardless of the Linux distribution. And is also the path to follow for
other systems (e.g. Windows).

Download the toolchain from
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

Unpack, for example:

    $ mkdir -p ~/opt
    $ cd ~/opt
    $ tar xjf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2

Add the toolchain binary path to your PATH environment variaable. Edit ``~/.bashrc``:

    export PATH="$HOME/opt/gcc-arm-none-eabi-9-2020-q2-update/bin:$PATH"

Exit the terminal and open a new one, so this change takes effect. Then verify:

    $ arm-none-eabi-gcc --version
    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)
    [...]


The terminal program
========================

Install pyserial system wide. This is required for pyterm to work; pyterm is
the terminal program included in RIOT. The terminal is the program you use
to interact with the application flashed to the board.

With Debian or Ubuntu :

    apt-get install python3-serial


Optional: direnv
========================

This is optional, but recommended: [direnv](https://direnv.net/) allows to
export environment variables as you enter a directory, and unload them as you
leave.

Install direnv in Debian or Ubuntu:

    apt-get install direnv

Enable direnv:

    direnv allow

This project includes a `.envrc` file in the root directory, it defines the
default port to be `/dev/ttyUSB0`


Documentation
====================

Our API is documented using doxygen. Just run doxygen from the root directory
of the project:

    doxygen

And then open your browser at the proper location,
``file:///[...]/wsn_riot/doc/html/index.html``


Arduino
====================

Most boards support Arduino, including native, e.g.:

    $ make BOARD=waspmote-pro
    There are unsatisfied feature requirements: arduino

Possibly it's not worth adding Arduino support because it will be too much
work, and not as good as going native.


TODO
=====================

Always check open issues and pull requests, what we need may be already there.

- move dprintf/dgets to helper module for reuse
- waspmote-pro: try power saving tests

Caveats:

- To exit the shell type "^D" then "ENTER". To implement an "exit" command we
  would need to modify RIOT.
