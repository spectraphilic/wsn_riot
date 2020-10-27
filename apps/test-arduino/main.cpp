#include "arduino_board.h"

#include <Wire.h>

#include <SparkFunBME280.h>

#define LIS3DH_CS 10

BME280 mySensorA; //Uses default I2C address 0x77

int main(void)
{
    Serial.begin(9600);
    Serial.println("Example showing alternate I2C addresses");

    Wire.begin();

    mySensorA.setI2CAddress(0x77); //The default for the SparkFun Environmental Combo board is 0x77 (jumper open).
    //If you close the jumper it is 0x76
    //The I2C address must be set before .begin() otherwise the cal values will fail to load.

    if(mySensorA.beginI2C() == false) Serial.println("Sensor A connect failed");

    while (1) {
        Serial.print("Humidity: ");
        Serial.print(mySensorA.readFloatHumidity(), 0);

        Serial.print(" Pressure: ");
        Serial.print(mySensorA.readFloatPressure(), 0);

        Serial.print(" Temp: ");
        Serial.print(mySensorA.readTempC(), 2);

        Serial.println();

        delay(1000);
    }

    return 0;
}
