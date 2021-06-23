/**
 * @file DHT22.h
 * @author Group 5
 * @brief Library for temperature and humidity values by using DHT22 sensor
 * Datasheet: @see https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
 * @version 1.0
 * @date 2021-06-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef DHT22_H
#define DHT22_H

#include <Arduino.h>
/**
 * Class for DHT22 sensor. 
 * Algorithm: https://www.teachmemicro.com/how-dht22-sensor-works/
 */
class DHT22
{
public:
    // Value returned when check errors during DHT22 processing
    enum DHT_ERROR_t
    {
        ERROR_NONE = 0, // No error
        ERROR_TIMEOUT,  // Timeout error
        ERROR_CHECKSUM  // Checksum errors
    };

    DHT22(int);              // Construct a new DHT22::DHT22 object
    void setup(int);         // Setup pin connected microcontroller
    void resetTimer();       // Reset timer for setup sensor
    DHT_ERROR_t getStatus(); // Check the sensor is working properly
    float getTemperature();  // Get  the temperature measured by the sensor
    float getHumidity();     // Get the humidity measured by the sensor

private:
    DHT_ERROR_t error;          // Error code
    unsigned long lastReadTime; // Last time the sensor was read
    float temperature;          // Temperature
    float humidity;             // Humidity
    uint8_t pin;                // Pin connected

    void readSensor(); // Read the measured sensor value
};

#endif // DHT22_H
