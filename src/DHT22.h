#ifndef DHT22_H
#define DHT22_H

#include <Arduino.h>

/**
 * @brief Library of DHT22 sensor
 * 
 */
class DHT22
{
public:
    /**
     * @brief Value returned when check errors during DHT22 processing
     * 
     */
    enum DHT_ERROR_t
    {
        ERROR_NONE = 0, // No error
        ERROR_TIMEOUT,  // Timeout error
        ERROR_CHECKSUM  // Checksum errors
    };

    /**
     * @brief Construct a new DHT22 object
     * 
     * @param pin pin connected microcontroller
     */
    DHT22(int);

    void setup(int);
    void resetTimer();

    DHT_ERROR_t getStatus() { return error; }
    float getTemperature();
    float getHumidity();

private:
    DHT_ERROR_t error;
    unsigned long lastReadTime;
    float temperature;
    float humidity;
    uint8_t pin;

    void readSensor();
};

#endif // DHT22_H
