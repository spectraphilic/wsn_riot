LoRa E5
=======


This document explains how to start working with the LoRa-E5 board from
SeedStudio. Links:

- [E5 mini](https://wiki.seeedstudio.com/LoRa_E5_mini/) from SeeedStudio
- [E5 development](https://wiki.seeedstudio.com/LoRa_E5_Dev_Board/) from SeeedStudio
- RIOT's [lora-e5-dev](https://doc.riot-os.org/group__boards__lora-e5-dev.html) board page
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
- In French: [Examples](https://stm32python.gitlab.io/fr-version-lora/lora-e5-mini.html)

Test the hardware
-----------------

Before starting it's probably good to verify the board is okay. As explained in
the official documentation from SeeedStudio
([here](https://wiki.seeedstudio.com/LoRa_E5_mini/#getting-started)):

1. Plug the board to the computer with a USB cable
2. Open a serial terminal (e.g. Arduino's)
3. Set the baudrate to 9600
4. Type some AT commands. This is an example of the inpu/output:

       AT
       +AT: OK

       AT+ID
       +ID: DevAddr, 42:00:7E:D2
       +ID: DevEui, 2C:F7:F1:20:42:00:7E:D2
       +ID: AppEui, 80:00:00:00:00:00:00:06

The ST-Link programmer
----------------------

To flash programs to the board a ST-Link v2 programmer is needed. Connect the
following pins, in the board and the programmer:

- GND
- SWCLK
- SWDIO
- RST

Some documents do not say to connect the reset pin (RST), but when I tried to
flash it didn't work if this pin was not connected.

Disable Read Out Protection
---------------------------

By default it is not possible to flash programs, first the Read Out Protection
must be disabled.

First download the [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html#get-software)
and install it.

Next plug both the board and the ST-Link programmer to the computer with a
couple USB cables.

Start the STM32CubeProgrammer software.

Click the *Connect* button:

- If there's an error then press the reset button in the board, click the
  *Connect* button, and release the board's reset button.

- The *Log* box will display the message `Error: Data read failed`, but it's
  okay, don't worry about this message.

<img src="https://files.seeedstudio.com/wiki/LoRa-E5_Development_Kit/wiki%20images/program1.png" width="800">

Now click the *OB* box in the left column, click on *Read Out Protection*, and
change the value to *AA*. Then click the *Apply* button.

<img src="https://files.seeedstudio.com/wiki/LoRa-E5_Development_Kit/wiki%20images/program2.png" width="800">

Flash a RIOT program
--------------------

To flash a program from RIOT a recent enough version of OpenOCD must be
installed, version 0.11.0 worked for me:

    $ openocd --version
    Open On-Chip Debugger 0.11.0

If this is good, try flashing a RIOT program:

    $ BAUD=115200 PORT=/dev/ttyUSB0 BOARD=lora-e5-dev make -C tests/leds all flash term
    [...]
    Info : clock speed 500 kHz
    Info : STLINK V2J29S7 (API v2) VID:PID 0483:3748
    Info : Target voltage: 3.271562
    Error: BUG: can't assert SRST
    Info : stm32wlx.cpu: hardware has 6 breakpoints, 4 watchpoints
    Info : starting gdb server for stm32wlx.cpu on 0
    Info : Listening on port 34183 for gdb connections
        TargetName         Type       Endian TapName            State
    --  ------------------ ---------- ------ ------------------ ------------
     0* stm32wlx.cpu       hla_target little stm32wlx.cpu       running

    Info : Unable to match requested speed 500 kHz, using 480 kHz
    Info : Unable to match requested speed 500 kHz, using 480 kHz
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x1fff2b6c msp: 0x20001508
    Info : device idcode = 0x10016497 (STM32WLEx - Rev 'unknown' : 0x1001)
    Info : flash size = 256kbytes
    Info : flash mode : single-bank
    Info : Padding image section 1 at 0x080023a4 with 4 bytes (bank write end alignment)
    Warn : Adding extra erase range, 0x080023a8 .. 0x080027ff
    auto erase enabled
    wrote 9128 bytes from file [...]/RIOT/examples/hello-world/bin/lora-e5-dev/hello-world.elf in 0.580517s (15.355 KiB/s)

    verified 9124 bytes in 0.343355s (25.950 KiB/s)

    Info : Unable to match requested speed 500 kHz, using 480 kHz
    Info : Unable to match requested speed 500 kHz, using 480 kHz
    shutdown command invoked
    Done flashing
    [...]

This program will flash the board's LED, and will print some messages.

- If no message is displayed, then probably the baudrate is wrong. I had to set
  the baudrate to 115200 to see the messages (the default is 9600).
