Toolchains
========================

You need to install a cross toolchain for every target architecture.


Native
------------------------

This allows running a RIOT program as a native Linux process, may be useful for
rapid development.

Debian:

    $ sudo apt install gcc-multilib g++-multilib

Test:

    $ cd RIOT/examples/default/
    $ make


AVR
------------------------

Debian:

    $ sudo apt install avr-libc gcc-avr avrdude

Gentoo (the USE flags are required to build avr-g++):

    $ sudo emerge crossdev
    $ USE="-openmp -hardened -sanitize -vtv" crossdev -S -t avr

Verify:

    $ avr-gcc --version
    $ avr-g++ --version

Test:

    $ BOARD=waspmote-pro make -C RIOT/examples/default/

Links:

- <https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega#atmel-avr-toolchain>
- <https://github.com/RIOT-OS/RIOT/issues/7109#issuecomment-305089485>


ARM
------------------------

Debian:

    $ sudo apt install gcc-arm-none-eabi

Verify:

    $ arm-none-eabi-gcc --version
    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)
    [...]

Test:

    $ BOARD=arduino-zero make -C RIOT/examples/default/


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
