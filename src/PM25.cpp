#include "PM25.h"

/**
 * @brief Construct a new PM25::PM25 object
 * @param analogPin Read value pin
 * @param ledPin Led IR pin
 */
PM25::PM25(int analogPin, int ledPin) : analogPin(analogPin), ledPin(ledPin) {}

/**
 * @brief Init pin mode for sensor
 */
void PM25::begin()
{
    pinMode(ledPin, OUTPUT);
}

/**
 * @brief Read the measured sensor value
 */
void PM25::readSensor()
{
    digitalWrite(ledPin, LOW); // Turn on IR
    delayMicroseconds(280);
    int rawData = analogRead(analogPin);

    delayMicroseconds(40);
    digitalWrite(ledPin, HIGH); // Turn off IR
    delayMicroseconds(9500);

    int voltage = voltageConversion(rawData);             // Convert from raw value to voltage
    dustDensity = (0.17 * (voltage + 0.45) - 0.1) * 1000; // Convert from voltage to dust density

    if (dustDensity < 0)
        dustDensity = NAN;
}

/**
 * @brief Get dust density of air
 * @return dust density (ug/m3)
 */
float PM25::getDustDensity()
{
    volatile int a;
    if (dustDensity == 0)
        readSensor();
    return dustDensity;
}

/**
 * @brief This function returns voltage from the raw input value
 * Refer ADC Conversion for further reference
 * @param value : value from analogPin
 * @return voltage
 */
float PM25::voltageConversion(int rawData)
{
    return (float)rawData * (3.3 / 4095.0);
}
