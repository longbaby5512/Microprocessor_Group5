/**
 * @file MQ7.h
 * @author Group 5
 * @brief Library for CO (Carbon Monoxide Gas) ppm value by using MQ7 gas sensor
 * Datasheet: @see https://www.sparkfun.com/datasheets/Sensors/Biometric/MQ-7.pdf
 * @version 1.0
 * @date 2021-06-18
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef MQ7_H
#define MQ7_H
#include <Arduino.h>

#define COEFFICIENT_A 19.32
#define COEFFICIENT_B -0.64

#define R_LOAD 10

/**
 * The coefficients are estimated from the sensitivity characteristics graph
 * of the MQ7 sensor for CO (Carbon Monoxide) gas by using Correlation function.
 * 
 * 
 * Explanation:
 * The graph in the datasheet is represented with the function f(x) = a * (x ^ b).
 * Where: f(x) = ppm, x = Rs/R0
 * 
 * The values were mapped with this function to determine the coefficients a and b.
 *
 */
class MQ7
{
private:
    uint8_t analogPin;
    uint8_t digitalPin;
    float voltageConversion(int); // Returns voltage from the raw input value

public:
    MQ7(uint8_t, uint8_t);       // Constructor to initialize the analog pin and input voltage to MQ7
    void begin();                // Setup pin
    float getPPM();              // Return the ppm value of CO gas
    float getSensorResistance(); // To find the sensor resistance Rs
    float getRatio();            // This function is for the deriving the Rs/R0 to find ppm
    int getLimit();              // Get limit
};

#endif // MQ7_H
