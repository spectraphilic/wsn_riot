# LoRa E5

@tableofcontents

This document explains how to start working with the LoRa-E5 board from
SeedStudio. Links:

- [E5 mini](https://wiki.seeedstudio.com/LoRa_E5_mini/) from SeeedStudio
- [E5 development](https://wiki.seeedstudio.com/LoRa_E5_Dev_Board/) from SeeedStudio
- RIOT's [lora-e5-dev](https://doc.riot-os.org/group__boards__lora-e5-dev.html) board page
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
- In French: [Examples](https://stm32python.gitlab.io/fr-version-lora/lora-e5-mini.html)

## Verify the hardware

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

## The ST-Link programmer

To flash programs to the board a ST-Link v2 programmer is needed. Connect the
following pins, in the board and the programmer:

- GND
- SWCLK
- SWDIO
- RST

Some documents do not say to connect the reset pin (RST), but when I tried to
flash it didn't work if this pin was not connected.

## Disable Read Out Protection

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

## Flash a RIOT program

To flash a program from RIOT a recent enough version of OpenOCD must be
installed, version 0.11.0 worked for me:

    $ openocd --version
    Open On-Chip Debugger 0.11.0

If this is good, try flashing a RIOT program:

    $ BOARD=lora-e5-dev make -C examples/hello-world all flash term
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


> **Note**
> Usually the first messages printed to the serial terminal are lost, so you may need to
> press the reset button after the program has been flashed, to see the whole sequence.

> **Note**
> It may also happen that you don't see anything at all even after pressing the reset
> button, this happens if the baudrate is incorrect. I had to pass `BAUD=115200` to be
> able to send/receive messages with the serial terminal.


## Testing

### LED and buttons

The mini board has one programmable LED (red) and one programmable button (Boot).
The development board has one more programmable button (D0).
We can use the `test/leds` program to test the LED and the *Boot* button:

    $ BOARD=lora-e5-dev make -C tests/leds all flash term
    [...]

First the LED will flick for a few seconds. Then it will light off, and while pressing the
*Boot* button it will light on.

Another program that can be tried is `test/buttons`, this will print a message every time
a button is pressed.

### Arduino

The `examples/arduino_hello-world/` will flicker the LED and echo anything typed in the
serial terminal.

The `tests/sys_arduino` program answers to a number of commands in the serial terminal:
`echo [...]`, `numb [...]`, `time` and `print`.

The `tests/periph_gpio_arduino/` program can be used for example to test the LED:

    > init_out 11
    > toggle 11

### I2C bus

In the development board, scanning the I2C bus with the `tests/periph_i2c` reveals a
device at address 0x48. This is the LM75 temperature sensor.

We can use the `tests/saul` program to verify the LM75 temperature sensor, and also
the programmable buttons (press a button to see the data line switch to 1):

    $ BOARD=lora-e5-dev make -C tests/saul all flash term
    [...]
    2022-10-25 09:35:06,911 # ##########################
    2022-10-25 09:35:07,891 #
    2022-10-25 09:35:07,892 # Dev: LED(red)	Type: ACT_SWITCH
    2022-10-25 09:35:07,892 # Data:	              0
    2022-10-25 09:35:07,892 #
    2022-10-25 09:35:07,897 # Dev: Button(B1 Boot)	Type: SENSE_BTN
    2022-10-25 09:35:07,898 # Data:	              0
    2022-10-25 09:35:07,898 #
    2022-10-25 09:35:07,904 # Dev: Button(B2 D0)	Type: SENSE_BTN
    2022-10-25 09:35:07,904 # Data:	              1
    2022-10-25 09:35:07,904 #
    2022-10-25 09:35:07,909 # Dev: lm75	Type: SENSE_TEMP
    2022-10-25 09:35:07,910 # Data:	          24.62 °C
    2022-10-25 09:35:07,910 #
    2022-10-25 09:35:07,911 # ##########################
    [...]

### SHT31

- Temperature & humidity
- Address 0x44
- RIOT module sht3x

With the `tests/periph_i2c` we can confirm the I2C address is 0x44:

    > i2c_scan 0
    2022-10-25 13:01:03,393 # i2c_scan 0
    2022-10-25 13:01:03,394 # Scanning I2C device 0...
    2022-10-25 13:01:03,399 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
    2022-10-25 13:01:03,406 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
    2022-10-25 13:01:03,407 # 0x00 R R R R R R R R R R R R R R - -
    2022-10-25 13:01:03,412 # 0x10 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,417 # 0x20 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,424 # 0x30 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,429 # 0x40 - - - - X - - - X - - - - - - -
    2022-10-25 13:01:03,436 # 0x50 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,444 # 0x60 - - - - - - - - - - - - - - - -
    2022-10-25 13:01:03,446 # 0x70 - - - - - - - - R R R R R R R R

Now we can test the sensor:

    $ CFLAGS="-DSHT3X_PARAM_I2C_ADDR=0x44" BOARD=lora-e5-dev make -C tests/driver_sht3x all flash term
    [...]
    2022-10-25 13:31:12,123 # +------------Initializing------------+
    2022-10-25 13:31:12,137 # Initialization successful
    2022-10-25 13:31:12,137 #
    2022-10-25 13:31:12,137 #
    2022-10-25 13:31:12,138 # +--------Starting Measurements--------+
    2022-10-25 13:31:12,154 # Temperature [°C]: 26.26
    2022-10-25 13:31:12,155 # Relative Humidity [%]: 54.55
    2022-10-25 13:31:12,158 # +-------------------------------------+
    2022-10-25 13:31:13,164 # Temperature [°C]: 26.23
    2022-10-25 13:31:13,164 # Relative Humidity [%]: 54.52
    2022-10-25 13:31:13,169 # +-------------------------------------+
    [...]

> **Note**
> By default the address used by the RIOT `sht3x` module is `0x45`, so we have to tell it
> to use `0x44`

We can as well try the SAUL test program:

    $ CFLAGS="-DSHT3X_PARAM_I2C_ADDR=0x44" USEMODULE=sht3x BOARD=lora-e5-dev make -C tests/saul all flash term
    [...]
    2022-10-25 13:37:04,443 # Dev: LED(red)	Type: ACT_SWITCH
    2022-10-25 13:37:04,444 # Data:	              0
    2022-10-25 13:37:04,444 #
    2022-10-25 13:37:04,449 # Dev: Button(B1 Boot)	Type: SENSE_BTN
    2022-10-25 13:37:04,449 # Data:	              0
    2022-10-25 13:37:04,449 #
    2022-10-25 13:37:04,455 # Dev: Button(B2 D0)	Type: SENSE_BTN
    2022-10-25 13:37:04,455 # Data:	              0
    2022-10-25 13:37:04,455 #
    2022-10-25 13:37:04,461 # Dev: lm75	Type: SENSE_TEMP
    2022-10-25 13:37:04,462 # Data:	          26.00 °C
    2022-10-25 13:37:04,462 #
    2022-10-25 13:37:04,468 # Dev: sht3x1	Type: SENSE_TEMP
    2022-10-25 13:37:04,468 # Data:	          26.26 °C
    2022-10-25 13:37:04,468 #
    2022-10-25 13:37:04,469 # Dev: sht3x1	Type: SENSE_HUM
    2022-10-25 13:37:04,474 # Data:	          54.64 %
    [...]
