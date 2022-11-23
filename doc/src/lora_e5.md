# LoRa E5

@tableofcontents

This document explains how to start working with the LoRa-E5 board from
SeedStudio. Links:

- [E5 mini](https://wiki.seeedstudio.com/LoRa_E5_mini/) from SeeedStudio
- [E5 development](https://wiki.seeedstudio.com/LoRa_E5_Dev_Board/) from SeeedStudio
- RIOT's [lora-e5-dev](https://doc.riot-os.org/group__boards__lora-e5-dev.html) board page
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
- In French: [Examples](https://stm32python.gitlab.io/fr-version-lora/lora-e5-mini.html)

> ARM Cortex-M4 core and Semtech SX126X LoRa chip, supports both LoRaWAN and LoRa protocol
> on the worldwide frequency and (G)FSK, BPSK, (G)MSK, and LoRa modulations.

## Verify the hardware

The board comes with a firmware preinstalled that answers to AT commands. Before we
override this firmware forever, use it to verify the board is okay, and get some info that
*may* be needed later for LoRaWAN.

As explained in the official documentation from SeeedStudio
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

In LoRaWAN networks, for OTAA activation, the device EUI (Extended Unique Identifier),
application EUI (aka JoinEUI), and application key are needed.

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

> **Warning**
> I have observed that `LED_OFF` switches **on** the LED, and `LED_ON` switches *off* the
> LED. **TODO** Submit an issue or pull-request to RIOT.

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

## LoRa

The chip has a SX126X module, so we can first test the sx126x driver:

    $ LORA_DRIVER=sx126x_stm32wl BOARD=lora-e5-dev make -C tests/driver_sx126x all flash term
    [...]
    > sx126x
    2022-11-04 10:18:53,948 # sx126x
    2022-11-04 10:18:53,949 # Usage: sx126x <get|set|rx|tx>
    [...]
    > sx126x get type
    2022-11-04 13:08:38,601 # sx126x get type
    2022-11-04 13:08:38,601 # Device type: lora
    > sx126x set freq 865200000
    > sx126x set bw 125
    > sx126x set cr 1
    > sx126x set sf 12
    > sx126x rx start
    [...]
    > sx126x rx stop
    > sx126x tx "Hello"

> **Warning**
> The type must be `lora` not `fsk`. For this you must pass the
> `LORA_DRIVER=sx126x_stm32wl` option.

For pint-to-point communication we can mix a lora-e5 (SX126X) with a waspmote (SX1272),
the feather-m0 has as well a LoRa shield (SX1276):

    Mode 1
    BW 125
    CR 4/5
    SF 12

When sending "ping" command in the waspmote, I get this in the lora-e5:

    2022-11-04 13:06:47,702 # Received: "" (9 bytes) - [RSSI: -63, SNR: 6, TOA: 992ms]

### LoRaWAN

With `tests/gnrc_lorawan`:

    $ BOARD=lora-e5-dev make -C tests/gnrc_lorawan all flash term
    [...]
    2022-11-04 09:52:01,246 # - gnrc_lorawan_tests
    2022-11-04 09:52:01,248 # 1) OK test_gnrc_lorawan__validate_mic
    2022-11-04 09:52:01,252 # 2) OK test_gnrc_lorawan__wrong_mic
    2022-11-04 09:52:01,264 # 3) OK test_gnrc_lorawan__build_hdr
    2022-11-04 09:52:01,266 # 4) OK test_gnrc_lorawan_fopts__mlme_link_check_req
    2022-11-04 09:52:01,268 # 5) OK test_gnrc_lorawan_fopts__perform
    2022-11-04 09:52:01,270 # 6) OK test_gnrc_lorawan_fopts__perform_wrong
    2022-11-04 09:52:01,271 #
    2022-11-04 09:52:01,271 # OK (6 tests)
    2022-11-04 09:52:01,274 # { "threads": [{ "name": "main", "stack_size": 1536, "stack_used": 852 }]}

With `tests/gnrc_lorawan_11`:

    $ BOARD=lora-e5-dev make -C tests/gnrc_lorawan_11 all flash term
    [...]
    2022-11-04 09:55:21,426 # - gnrc_lorawan_tests
    2022-11-04 09:55:21,428 # 1) OK test_gnrc_lorawan__validate_mic
    2022-11-04 09:55:21,432 # 2) OK test_gnrc_lorawan__wrong_mic
    2022-11-04 09:55:21,437 # 3) OK test_gnrc_lorawan__build_hdr
    2022-11-04 09:55:21,439 # 4) OK test_gnrc_lorawan_fopts__mlme_link_check_req
    2022-11-04 09:55:21,444 # 5) OK test_gnrc_lorawan_fopts__perform
    2022-11-04 09:55:21,449 # 6) OK test_gnrc_lorawan_fopts__perform_wrong
    2022-11-04 09:55:21,449 #
    2022-11-04 09:55:21,450 # OK (6 tests)
    2022-11-04 09:55:21,454 # { "threads": [{ "name": "main", "stack_size": 1536, "stack_used": 900 }]}

To test LoRaWAN we can register to TTN (The Things Network), create a new application,
register our device

The use the `tests/pkg_semtech-loramac` program:

    $ LORA_DRIVER=sx126x_stm32wl BOARD=lora-e5-dev make -C tests/pkg_semtech-loramac all flash term
    [...]
    > loramac set deveui 2CF7F12042007ED2
    2022-11-04 12:47:32,365 # loramac set deveui 2CF7F12042007ED2
    > loramac set appeui 8000000000000006
    2022-11-04 12:48:02,040 # loramac set appeui 8000000000000006
    > loramac set appkey 7B20478B27A026A6CD384C1CFA259DCA
    2022-11-04 12:48:18,444 # loramac set appkey 7B20478B27A026A6CD384C1CFA259DCA
    > loramac join otaa
    2022-11-04 12:48:30,055 # loramac join otaa
    2022-11-04 12:48:38,061 # Join procedure failed!

Or the `examples/gnrc_lorawan` program:

    $ BOARD=lora-e5-dev make -C examples/gnrc_lorawan all flash term
    [...]
    > ifconfig
    2022-11-04 11:02:13,565 # ifconfig
    2022-11-04 11:02:13,571 # Iface  3  HWaddr: 00:00:00:00  Frequency: 868300000Hz  RSSI: -128  BW: 125kHz  SF: 7  CR: 4/5  Link: down
    2022-11-04 11:02:13,577 #            State: STANDBY  Demod margin.: 0  Num gateways.: 0
    2022-11-04 11:02:13,580 #           OTAA
    2022-11-04 11:02:13,580 #
    > ifconfig 3 set deveui 2CF7F12042007ED2
    > ifconfig 3 set appeui 8000000000000006
    > ifconfig 3 set appkey 7B20478B27A026A6CD384C1CFA259DCA
    > ifconfig 3 up

> **Note**
> This did not work for me yet.

Links:

- <https://wiki.seeedstudio.com/Grove_LoRa_E5/#ttn-console-configuration-setup>
- https://github.com/RIOT-OS/RIOT/issues/17059

## External modules

These are some modules we are trying.

> **Warning**
> This sections is still very much in progress!

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


### The DS1307 RTC

> **Warning**
> I ordered the wrong clock from SeeedStudio, a PCF85063TP which is close to the DS1307
> but not enough.

- Button battery CR1225
- Address 0x51
- RIOT module ds1307 (with address 0x68)

With the `tests/periph_i2c` we can confirm the I2C address is 0x51:

    > i2c_scan 0
    2022-10-26 09:31:44,457 # i2c_scan 0
    2022-10-26 09:31:44,457 # Scanning I2C device 0...
    2022-10-26 09:31:44,463 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
    2022-10-26 09:31:44,470 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
    2022-10-26 09:31:44,470 # 0x00 R R R R R R R R R R R R R R - -
    2022-10-26 09:31:44,475 # 0x10 - - - - - - - - - - - - - - - -
    2022-10-26 09:31:44,481 # 0x20 - - - - - - - - - - - - - - - -
    2022-10-26 09:31:44,488 # 0x30 - - - - - - - - - - - - - - - -
    2022-10-26 09:31:44,493 # 0x40 - - - - X - - - X - - - - - - -
    2022-10-26 09:31:44,499 # 0x50 - X - - - - - - - - - - - - - -
    2022-10-26 09:31:44,508 # 0x60 - - - - - - - - - - - - - - - -
    2022-10-26 09:31:44,509 # 0x70 - - - - - - - - R R R R R R R R

In RIOT the device address is hardcoded to 0x68, so the RIOT's source file
`drivers/include/ds1307.h` must be edited to change the address to 0x51:

    #define DS1307_I2C_ADDRESS      (0x51)

### The DS3231 RTC

The Adafruit FeatherWing DS3231 Precision RTC includes:

- RTC DS3231
- CR1220 button battery

See https://learn.adafruit.com/ds3231-precision-rtc-featherwing

Wiring:

    GND                   Black
    VCC 3V3               Red
    SCL                   Yellow
    SDA                   Blue (or Green)
    SQW/INT (optional)    ?                 DS3231_PARAM_INT_PIN

I2C address: 0x68

Test:

    $ BOARD=lora-e5-dev make -C tests/periph_i2c all flash term
    [...]
    > 2022-11-03 11:37:29,883 # i2c_scan 0
    2022-11-03 11:37:29,884 # Scanning I2C device 0...
    2022-11-03 11:37:29,888 # addr not ack'ed = "-", addr ack'ed = "X", addr reserved = "R", error = "E"
    2022-11-03 11:37:29,895 #      0 1 2 3 4 5 6 7 8 9 a b c d e f
    2022-11-03 11:37:29,896 # 0x00 R R R R R R R R R R R R R R - -
    2022-11-03 11:37:29,902 # 0x10 - - - - - - - - - - - - - - - -
    2022-11-03 11:37:29,911 # 0x20 - - - - - - - - - - - - - - - -
    2022-11-03 11:37:29,913 # 0x30 - - - - - - - - - - - - - - - -
    2022-11-03 11:37:29,920 # 0x40 - - - - - - - - X - - - - - - -
    2022-11-03 11:37:29,925 # 0x50 - - - - - - - - - - - - - - - -
    2022-11-03 11:37:29,933 # 0x60 - - - - - - - - X - - - - - - -
    2022-11-03 11:37:29,936 # 0x70 - - - - - - - - R R R R R R R R


### SD card reader

The Adalogger FeatherWing includes:

- SD card reader
- RTC PCF8523 (I2C address 0x68)
- CR1220 button battery

See https://learn.adafruit.com/adafruit-adalogger-featherwing

> **Warning**
> The I2C address 0x68 is the same used by the DS3231 and DS1307.

> **Note**
> I tried with this module as well
> https://circuitstate.com/tutorials/interfacing-catalex-micro-sd-card-module-with-arduino/
> but it didn't work. Apparently the pins value are at 5V, designed for Arduino, so it
> does not work with 3V3 MCUs.

Wiring:

- GND   Black
- VCC   Red
- CLK   Yellow
- MOSI  Green
- MISO  Blue
- CS    Orange

Test:

    $ BOARD=lora-e5-dev make -C tests/driver_sdcard_spi all flash term
    [...]
    > init
    2022-10-31 12:18:18,562 # init
    2022-10-31 12:18:18,563 # Initializing SD-card at SPI_0...SD_INIT_START
    2022-10-31 12:18:18,566 # gpio_init(): [OK]
    2022-10-31 12:18:18,566 # SD_INIT_SPI_POWER_SEQ
    2022-10-31 12:18:18,574 # SD_INIT_SEND_CMD0
    2022-10-31 12:18:18,581 # sdcard_spi_send_cmd: CMD00 (0x00000000) (remaining retry time 88 usec)
    2022-10-31 12:18:18,581 # _wait_for_not_busy: [OK]
    2022-10-31 12:18:18,588 # CMD00 echo: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
    2022-10-31 12:18:18,593 # _wait_for_r1: r1=0xff
    2022-10-31 12:18:18,594 # _wait_for_r1: r1=0x01
    2022-10-31 12:18:18,594 # _wait_for_r1: R1_VALID
    2022-10-31 12:18:18,594 # CMD0: [OK]
    2022-10-31 12:18:18,599 # SD_INIT_ENABLE_CRC
    2022-10-31 12:18:18,605 # sdcard_spi_send_cmd: CMD59 (0x00000001) (remaining retry time 249988 usec)
    2022-10-31 12:18:18,606 # _wait_for_not_busy: [OK]
    2022-10-31 12:18:18,611 # CMD59 echo: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
    2022-10-31 12:18:18,611 # _wait_for_r1: r1=0xff
    2022-10-31 12:18:18,617 # _wait_for_r1: r1=0xff
    [...]
    2022-10-31 12:18:18,949 # _wait_for_r1: r1=0xff
    2022-10-31 12:18:18,949 # _wait_for_r1: [TIMEOUT]
    2022-10-31 12:18:18,954 # sdcard_spi_send_cmd: R1_TIMEOUT (0xff)
    2022-10-31 12:18:18,955 # SD_INIT_CARD_UNKNOWN
    2022-10-31 12:18:18,955 # [FAILED]

With the remote-revb:

    > init
    2022-10-31 12:34:16,081 # init
    2022-10-31 12:34:16,082 # Initializing SD-card at SPI_1...SD_INIT_START
    2022-10-31 12:34:16,082 # gpio_init(): [OK]
    2022-10-31 12:34:16,082 # SD_INIT_SPI_POWER_SEQ
    2022-10-31 12:34:16,094 # SD_INIT_SEND_CMD0
    2022-10-31 12:34:16,100 # sdcard_spi_send_cmd: CMD00 (0x00000000) (remaining retry time 69 usec)
    2022-10-31 12:34:16,103 # _wait_for_not_busy: [OK]
    2022-10-31 12:34:16,115 # CMD00 echo: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
    2022-10-31 12:34:16,116 # _wait_for_r1: r1=0xff
    2022-10-31 12:34:16,124 # _wait_for_r1: r1=0x01
    2022-10-31 12:34:16,124 # _wait_for_r1: R1_VALID
    2022-10-31 12:34:16,125 # CMD0: [OK]
    2022-10-31 12:34:16,125 # SD_INIT_ENABLE_CRC
    2022-10-31 12:34:16,134 # sdcard_spi_send_cmd: CMD59 (0x00000001) (remaining retry time 249969 usec)
    2022-10-31 12:34:16,134 # _wait_for_not_busy: [OK]
    2022-10-31 12:34:16,146 # CMD59 echo: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
    2022-10-31 12:34:16,146 # _wait_for_r1: r1=0xff
    2022-10-31 12:34:16,146 # _wait_for_r1: r1=0x01
    2022-10-31 12:34:16,146 # _wait_for_r1: R1_VALID
    2022-10-31 12:34:16,147 # CMD59: [OK]
    [...]


### GPS (Grove - GPS Air530)

- Air530Z chip
- Button battery CR1220

> **Warning**
> The Grove connetor does not work because it's plugged to USART1, which is used for
> serial communication with the computer through USB-C. So we have to plug it to USART2.
> Though maybe when deployed USART1 could be used, since the board won't be plugged to the
> computer.

Test UART communication:

    $ BOARD=lora-e5-dev make -C tests/periph_uart all flash term
    [...]
    > init 1 9600
    2022-11-22 13:09:44,186 # init 1 9600
    2022-11-22 13:09:44,187 # Success: Initialized UART_DEV(1) at BAUD 9600
    2022-11-22 13:09:44,440 # UARD_DEV(1): test uart_poweron() and uart_poweroff()  ->  [OK]
    > 2022-11-22 13:09:44,868 # Success: UART_DEV(1) RX: [$GNGGA,,,,,,0,00,25.5,,,,,,*640x0d]\n
    2022-11-22 13:09:44,889 # Success: UART_DEV(1) RX: [$GNGLL,,,,,,V,N*7A0x0d]\n
    2022-11-22 13:09:44,936 # Success: UART_DEV(1) RX: [$GNGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5,1*010x0d]\n
    2022-11-22 13:09:44,982 # Success: UART_DEV(1) RX: [$GNGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5,4*040x0d]\n
    2022-11-22 13:09:45,002 # Success: UART_DEV(1) RX: [$GPGSV,1,1,00,0*650x0d]\n
    [...]

Or with our shell application:

    $ make -C apps/wsn-shell all flash term
    [...]
    > gps
    2022-11-22 13:25:49,351 # gps
    2022-11-22 13:25:49,358 # time=6.879 level=info thread=main UART 1 initialized bauds=9600 err=0
    2022-11-22 13:25:49,359 # time=6.886 level=info thread=main GPS on
    2022-11-22 13:25:49,699 # RX $GNGGA,,,,,,0,00,25.5,,,,,,*64\r\n
    2022-11-22 13:25:49,718 # RX $GNGLL,,,,,,V,N*7A\r\n
    2022-11-22 13:25:49,767 # RX $GNGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5,1*01\r\n
    [...]

Links:

- <https://www.seeedstudio.com/Grove-GPS-Air530-p-4584.html>
- <https://wiki.seeedstudio.com/Grove-GPS-Air530/>
- <https://github.com/sivaelid/Heltec_AB02S_Mods>


### SDI-12

Tested RIOT's `tests/pkg_arduino_sdi_12` program with the Decagon CTD-10 sensor, which
uses 5V power.  Only needed to change the pin in the `main.c` file, to use pin 21 instead
of 13.

> **Warning**
> The board is not compatible with Arduino, but support for Arduino in RIOT was added to
> it. This means that the pin numbers do not match those of Arduino. The exact definition
> of the Arduino pin numbers for the lora-e5-dev board can be found in
> `boards/lora-e5-dev/include/arduino_pinmap.h`. Here we use pin 21 which in the board is
> labelled as **D9**.

This is the proram's output:

    $ BOARD=lora-e5-dev make -C tests/pkg_arduino_sdi_12 all flash term
    [...]
    2022-11-09 13:30:46,086 # Testing the Arduino-SDI-12 package
    2022-11-09 13:30:46,086 #
    2022-11-09 13:30:46,087 # Send: ?!
    2022-11-09 13:30:46,087 # Recv: 0
    2022-11-09 13:30:46,088 # Send: 0I!
    2022-11-09 13:30:46,330 # Recv: 013DECAGON CTD-103991059303507
    2022-11-09 13:30:46,330 # Send: 0M!
    2022-11-09 13:30:48,676 # Recv: 00013
    2022-11-09 13:30:49,677 # Send: 0D0!
    2022-11-09 13:30:50,034 # Recv: 0+35+23.1+0
