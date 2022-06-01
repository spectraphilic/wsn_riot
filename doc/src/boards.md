# Boards

## Zolertia's remote-revb board

This is the board we have worked with the most, and so the best supported so
far.

Up:

- Includes an RTC, SD card, and radio in the board.

Down:

- The board is out-of-stock
- Missing driver for the RTC, see https://github.com/spectraphilic/wsn\_riot/issues/8
- Missing driver for the CC1200 radio, see https://github.com/spectraphilic/wsn\_riot/issues/12


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
