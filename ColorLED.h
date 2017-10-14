#ifndef COLORLED_H
#define COLORLED_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class ColorLED
{
  public:
    ColorLED(int pin, int num);
    void shine(unsigned char r, unsigned char g, unsigned char b, unsigned char brightness);
  private:
    Adafruit_NeoPixel led;
    int number;
};

#endif

