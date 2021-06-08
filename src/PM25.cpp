#include "PM25.h"

PM25::PM25(int measurePin, int ledPower) : measurePin(measurePin), ledPower(ledPower) {}

void PM25::begin()
{
    pinMode(ledPower, OUTPUT);
}

void PM25::readSensor()
{
    digitalWrite(ledPower, LOW);     //Bật IR
    delayMicroseconds(samplingTime); //Chờ

    voMeasured = analogRead(measurePin); //Đọc giá trị vào

    delayMicroseconds(deltaTime); //Chờ
    digitalWrite(ledPower, HIGH); //Tắt IR
    delayMicroseconds(sleepTime); //Chờ

    calcVoltage = voMeasured * (5.0 / 1024); //Biến đổi sang dòng điện
    dustDensity = 0.17 * calcVoltage - 0.1;  //Biến đổi sang mật độ bụi

    if (dustDensity < 0)
    {
        dustDensity = 0.00;
    }
}

float PM25::getDustDensity()
{
    while (1)
    {
        if (dustDensity != 0)
            break;
        readSensor();
    }
    return dustDensity;
}
