#ifndef RN4020_H
#define RN4020_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RN4020
{
  public:
    RN4020(int pinRx, int pinTx, int baudRate);
    void begin();
    void send(String str);
    String receive();
    void reboot();
    void changeBaudrate(int baudRate);
  private:
    SoftwareSerial serial;
    int baudRate;
};

#endif

