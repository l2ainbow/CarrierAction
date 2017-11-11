#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

const int AVERAGING_TIME = 5;
const int THRESHOLD_CYCLE = 400;
const int BUFFER_ELEMENT = 100;

class Encoder
{
  
  public:
    Encoder(int pin);
    void encode();
    float getSpeed();
  private:
    int pin;
    float speed;
    int cnt;
    int valueBuffer[BUFFER_ELEMENT];
    int timeBuffer[BUFFER_ELEMENT];
    int averageBuffer[BUFFER_ELEMENT];
    int movingAverage(int);
};

#endif

