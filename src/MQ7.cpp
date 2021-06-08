#include "MQ7.h"

void MQ7::begin()
{
    pinMode(pinDigital, INPUT);
}

void MQ7::readSensor()
{
    limit = digitalRead(pinDigital);
    // Kiem tra chan DOUT của Module
    if (digitalRead(pinAnalog) == LOW) // Neu vuot nguong
        value = analogRead(pinAnalog);
    else // Neu khong vuot nguong
        value = 0.00;
}

int MQ7::getCOValue()
{
    readSensor();
    return value;
}

bool MQ7::checkHadCO()
{
    readSensor();
    return limit;
}
