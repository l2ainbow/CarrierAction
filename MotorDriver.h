#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>

class MotorDriver
{
  // モータのブースト時間（停止〜加速）[ms]
  static const int BOOST_TIME = 50;
  public:
    MotorDriver(int pin1, int pin2);
    void rotate(int pwm);
    static bool hasBoosted;
    int getPWM();
  private:
    static void stopBoosting();
    void rotateAgain();
    int pin1;
    int pin2;
    int pwm;
};

#endif


