#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>

class MotorDriver
{
  public:
    MotorDriver(int pin1, int pin2);
    void rotate(int pwm);
    int getPWM();
  private:
    int pin1;
    int pin2;
    int pwm;
};

#endif


