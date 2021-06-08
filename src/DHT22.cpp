#include "DHT22.h"

DHT22::DHT22(int pin) : pin(pin) {}

/**
 * @brief Setup pin connected microcontroller
 * 
 * @param pin pin connected microcontroller
 */
void DHT22::setup(int pin)
{
    this->pin = pin;
    resetTimer();
}

void DHT22::resetTimer()
{
    lastReadTime = millis() - 3000;
}

void DHT22::readSensor()
{
    // Make sure we don't poll the sensor too often
    // - Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
    // - Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)
    unsigned long startTime = millis();
    if ((unsigned long)(startTime - lastReadTime) < 1999L)
    {
        return;
    }
    lastReadTime = startTime;

    temperature = NAN;
    humidity = NAN;

    uint16_t rawHumidity = 0;
    uint16_t rawTemperature = 0;
    uint16_t data = 0;

    // Request sample
    digitalWrite(pin, LOW); // Send start signal
    pinMode(pin, OUTPUT);

    delayMicroseconds(2e3);

    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH); // Switch bus to receive data

    // We're going to read 83 edges:
    // - First a FALLING, RISING, and FALLING edge for the start bit
    // - Then 40 bits: RISING and then a FALLING edge per bit
    // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

#ifdef ESP32
    // ESP32 is a multi core / multi processing chip
    // It is necessary to disable task switches during the readings
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
#else
    //   cli();
    noInterrupts();
#endif
    for (int8_t i = -3; i < 2 * 40; i++)
    {
        byte age;
        startTime = micros();

        do
        {
            age = (unsigned long)(micros() - startTime);
            if (age > 90)
            {
                error = ERROR_TIMEOUT;
#ifdef ESP32
                portEXIT_CRITICAL(&mux);
#else
                // sei();
                interrupts();
#endif
                return;
            }
        } while (digitalRead(pin) == (i & 1) ? HIGH : LOW);

        if (i >= 0 && (i & 1))
        {
            // Now we are being fed our 40 bits
            data <<= 1;

            // A zero max 30 usecs, a one at least 68 usecs.
            if (age > 30)
            {
                data |= 1; // we got a one
            }
        }

        switch (i)
        {
        case 31:
            rawHumidity = data;
            break;
        case 63:
            rawTemperature = data;
            data = 0;
            break;
        }
    }

#ifdef ESP32
    portEXIT_CRITICAL(&mux);
#else
    //   sei();
    interrupts();
#endif

    // Verify checksum

    if ((byte)(((byte)rawHumidity) + (rawHumidity >> 8) + ((byte)rawTemperature) + (rawTemperature >> 8)) != data)
    {
        error = ERROR_CHECKSUM;
        return;
    }

    // Store readings

    humidity = rawHumidity * 0.1;

    if (rawTemperature & 0x8000)
        rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);

    temperature = ((int16_t)rawTemperature) * 0.1;

    error = ERROR_NONE;
}

float DHT22::getTemperature()
{
    readSensor();
    if (error == ERROR_TIMEOUT)
        readSensor();
    return temperature;
}

float DHT22::getHumidity()
{
    readSensor();
    if (error == ERROR_TIMEOUT)
        readSensor();
    return humidity;
}
