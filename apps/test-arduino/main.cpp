#include <fmt.h>

#include <arduino_board.h>
#include <Wire.h>

#include <SparkFunBME280.h>


BME280 mySensor;

int main(void)
{
    int error = 0;

    Serial.begin(9600);
    Serial.println("Read samples from Sparkfun's BME280 at 0x77 I2C address");

#ifdef CPU_ATMEGA1281
    // This should be ifdef BOARD_WASPMOTE_PRO but it doesn't work: open issue?
    // Switch on 3v3
    pinMode(26, OUTPUT);
    digitalWrite(26, HIGH);
    delay(100);
#endif

    Wire.begin();

    // The default for the SparkFun Environmental Combo board is 0x77 (jumper open).
    // If you close the jumper it is 0x76
    // The I2C address must be set before .begin() otherwise the cal values will fail to load.
    mySensor.setI2CAddress(0x77);

    if (mySensor.beginI2C() == false) {
        Serial.println("Sensor 0x77 connect failed");
        error = -1;
        goto exit;
    }

    while (1) {
        Serial.print("Humidity: ");
        print_float(mySensor.readFloatHumidity(), 0);
        //Serial.print(mySensor.readFloatHumidity(), 0);

        Serial.print(" Pressure: ");
        print_float(mySensor.readFloatPressure(), 0);
        //Serial.print(mySensor.readFloatPressure(), 0);

        Serial.print(" Temp: ");
        print_float(mySensor.readTempC(), 2);
        //Serial.print(mySensor.readTempC(), 2);

        Serial.println();

        delay(3000);
    }

exit:

#ifdef CPU_ATMEGA1281
    digitalWrite(26, LOW);
#endif

    return error;
}
