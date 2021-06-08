#ifndef MQ7_H
#define MQ7_H
#include <Arduino.h>

class MQ7
{
private:
    int pinAnalog;
    int pinDigital;
    int limit;
    int value = 0;

    void readSensor();

public:
    MQ7(int pinAnalog, int pinDigital) : pinAnalog(pinAnalog), pinDigital(pinDigital) {}
    void begin();
    int getCOValue();
    bool checkHadCO();
};

#endif //MQ7_H
