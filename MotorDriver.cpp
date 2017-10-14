/**
  モータドライバのクラス
  author: Yu Iijima
 **/

#include "MotorDriver.h"
#include <Arduino.h>

// コンストラクタ
// pin1: モータドライバの入力端子1と接続しているArduinoのPIN番号
// pin2: モータドライバの入力端子2と接続しているArduinoのPIN番号
MotorDriver::MotorDriver(int pin1, int pin2)
:
  pin1(pin1),
  pin2(pin2),
  pwm(0)
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
}

// モータを回転する
// pwm: モータにかける電圧のPWM (-100(後進) ~ 100(前進))
void MotorDriver::rotate(int pwm) {
  this->pwm = pwm;

  boolean isAhead = (pwm > 0);
  boolean isBreak = (pwm == 0);

  int val = (int)(abs(pwm) / 100.0 * 255);

  if (isBreak) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
  }
  else if (isAhead) {
    analogWrite(pin1, 0);
    analogWrite(pin2, val);
  }
  else {
    analogWrite(pin1, val);
    analogWrite(pin2, 0);
  }
}

// PWM値の取得
// -> PWM値
int MotorDriver::getPWM(){
  return pwm;
}


