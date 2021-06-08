#ifndef PM25_H
#define PM25_H
#include <Arduino.h>

class PM25
{
public:
    PM25(int, int);
    void begin();

    float getDustDensity();

private:
    int measurePin; //Chân đọc giá trị
    int ledPower;   //Chân led IR

    float voMeasured = 0;  //Giá trị thô (analog)
    float calcVoltage = 0; //Giá trị điện áp
    float dustDensity = 0;

    unsigned int samplingTime = 280; //Thời gian chờ
    unsigned int deltaTime = 40;
    unsigned int sleepTime = 9680;

    void readSensor();
};

#endif //PM25GP2Y10_H
