# Boards

@tableofcontents

Our new generation datalogger is based on the lora-e5 board, but before we tried several
other boards:

- Zolertia's remote-revb board is fine, but has been out of stock for a long time. Also
  it has a 802.15.4 radio, not LoRa.

- Adafruit Feather-M0 has proven to be fragile and unrelible.

- The Arduino Zero was useful to test RIOT support for Arduino code. It has the same MCU
  as the Feather-M0.

- Waspmote and other AVR boards. We have a stock of waspmotes that would be nice to reuse.
  The main problem with AVR boards is they have too little memory.

- iM880B is an old board, we have used it only to debug a problem with the SPI bus. It has
  a LoRa module.


## LoRa-E5

See [LoRa E5](lora_e5.md) page.


## Adafruit Feather M0 (feather-m0)

Specifically we have these boards and these shields:

- Feather M0 Adalogger https://www.adafruit.com/product/2796
- DS3231 Precision RTC FeatherWing https://www.adafruit.com/product/3028
- LoRa Radio FeatherWing https://www.adafruit.com/product/3231

Flashing to the feather-m0 may not work, for example:

    $ BOARD=feather-m0 make -C examples/arduino_hello-world all flash term
    [...]
    sleep 2
    [...]/bossac [...] [...]examples/arduino_hello-world/bin/feather-m0/arduino_hello-world.bin
    No device found on /dev/ttyACM0

To fix it you have to double click the reset button while the program is
flashed, exactly when the ``sleep 2`` message is displayed.

If a RIOT program is already on the microprocessor and it contains a shell component, it can be accessed with (as Feather M0 on linux are loaded on port `/dev/ttyACM0`)

```
RIOT/dist/tools/pyterm/pyterm -p "/dev/ttyACM0" -b "115200"
```



Up:

- Available and affordable
- Several combinations possible with different shields


## Arduino Zero (arduino-zero)

To flash programs to the arduino-zero board install openocd, see
``https://api.riot-os.org/group__boards__arduino-zero.html``

The board has 2 USB ports: the programming port and the native port.
Use the programming port, the one close to the supply connector.


## AVR based boards

AVR based boards such as the waspmote-pro and the arduino-mega2560 are unlikely
to work for us, because the program won't fit in the memory.

When flashing, if you get some error like:

    avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x31

Try unplugging and plugging again the USB cable.

## iM880B

The PCB includes a iM880B package and some other modules:

    MCU     : STM32L151CxU6Axx
    LoRa    : SX1272
    +Sensor : DS75LX
    +USB    : CH340G (requires USB_SERIAL_CH341 linux module)

Test:

    $ PORT=/dev/ttyUSB0 BOARD=im880b make -C tests/periph_spi term
    > init 1 0 0
    2023-01-18 11:41:57,394 # init 1 0 0
    2023-01-18 11:41:57,400 # SPI_DEV(1) initialized: mode: 0, clk: 0, cs_port: 0, cs_pin: 0

How to switch to programming mode and flash:

- Hold the reset button
- Connect the resistor loose pin to the the middle header (Vcc indicated by an arrow on
  the board)
- Release the reset button
- Flash:

    $ PROGRAMMER=stm32flash PORT=/dev/ttyUSB0 BOARD=im880b make -C tests/periph_spi flash

- Release the resistor loose pin
- Push the reboot button
- Enter the terminal
