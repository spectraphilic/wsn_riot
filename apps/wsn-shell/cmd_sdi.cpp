#ifdef MODULE_ARDUINO_SDI_12

#include <assert.h>
#include <stdio.h>

//#include <arduino_board.h>

#include <SDI12.h>

#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 13
#endif

SDI12 sdi12(SDI12_DATA_PIN);


int cmd_sdi(int argc, char **argv) {
    assert(argc); // Avoids warning
    assert(argv); // Avoids warning

    sdi12.begin();
    delay(500);  // allow things to settle

    sdi12.sendCommand("?I!");
    delay(300);                  // wait a while for a response
    while (sdi12.available())    // write the response to the screen
        Serial.write(sdi12.read());

    sdi12.end();

    return 0;
}


#endif
