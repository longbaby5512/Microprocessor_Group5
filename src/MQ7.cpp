#include "MQ7.h"

/**
 * @brief Constructor to initialize the analog pin and digital pin to MQ7
 * @param analogPin Input pin from MQ7 analog pin
 * @param digitalPin Input pin from MQ7 digital pin
 */
MQ7::MQ7(uint8_t analogPin, uint8_t digitalPin)
{
    this->digitalPin = digitalPin;
    this->analogPin = analogPin;
}

/**
 * @brief Setup pin
 */
void MQ7::begin()
{
    pinMode(digitalPin, INPUT);
}

/**
 * @brief Function is used to return the ppm value of CO gas concentration
by using the parameter found using the function f(x) = a * ((Rs/R0) ^ b)
 * @return ppm value of Carbon Monoxide concentration
 */
float MQ7::getPPM()
{
    if (analogRead(analogPin) == 0)
        return NAN;
    return (float)(COEFFICIENT_A * pow(getRatio(), COEFFICIENT_B));
}

/**
 * @brief This function returns voltage from the raw input value
 * Refer ADC Conversion for further reference
 * @param value : value from analogPin
 * @return voltage
 */
float MQ7::voltageConversion(int rawData)
{
    return (float)rawData * (3.3 / 4095.0);
}

/**
 * @brief This function is for the deriving the Rs/R0 to find ppm
 * @return The value of Rs/R_Load
 */
float MQ7::getRatio()
{
    float voltage = voltageConversion(analogRead(analogPin));
    return (3.3 - voltage) / voltage;
}

int MQ7::getLimit()
{
    return digitalRead(digitalPin);
}

/**
 * @brief To find the sensor resistance Rs
 * @return Rs value
 */
float MQ7::getSensorResistance()
{
    return R_LOAD * getRatio();
}
