/**
 * @file PM25.h
 * @author Group 5
 * @brief Library for dust density value by using PM2.5 GP2Y1010AU0F sensor
 * Datasheet: @see https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
 * @version 1.0
 * @date 2021-06-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef PM25_H
#define PM25_H
#include <Arduino.h>

#define R_LOAD 30.0

class PM25
{
public:
    PM25(int, int);         // Construct a new PM25::PM25 object
    void begin();           //Init pin mode for sensor
    float getDustDensity(); //Get dust density of air
    int getRawValue();      //Get raw value
    float getVoltage();     //Get voltage

private:
    int analogPin; // Read value pin
    int ledPin;    // LED IR pin

    float dustDensity = 0; // Dust density value

    void readSensor();            // Read the measured sensor value
    float voltageConversion(int); // Returns voltage from the raw input value
};

#endif //PM25GP2Y10_H
