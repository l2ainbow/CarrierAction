/**
  モータドライバのクラス
  author: Yu Iijima
 **/

#include "MotorDriver.h"
#include <Arduino.h>

const int NUM_PS = 9;

// pwmとspeedの関係を示す構造体
struct ps {
  float pwm;
  float speed;
};

// pwmとspeedの関係
struct ps ps[NUM_PS] = {
  {20,0},
  {30,28},
  {40,39},
  {50,59},
  {60,62},
  {70,74},
  {80,76},
  {90,86},
  {100,100}
};

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
void MotorDriver::rotate(float pwm) {
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

// モータを回転する
// speed: モータの回転速度（100を最大回転速度とした時の-100~100までの値）
void MotorDriver::rotateWithSpeed(float speed){
  float pwm = convertToPWM(abs(speed));
  this->rotate(pwm);
}

// 速度をPWMに変換する
// speed: モータの回転速度（100を最大回転速度とした時の-100~100までの値）
// -> PWMの値
float MotorDriver::convertToPWM(float speed){
  if (speed == 0){
    return 0;
  }
  
  float a_speed = abs(speed);

  float pwm = 0;
  for (int i = 1; i < NUM_PS; i++){
    if (a_speed <= ps[i].speed){
      float a = (ps[i].pwm - ps[i-1].pwm) / (ps[i].speed - ps[i-1].speed);
      float b = ps[i].pwm - a * ps[i].speed;
      pwm = a * a_speed + b;
      break;
    }
  }
  
  if (speed < 0){
    pwm = -pwm;
  }
  
  return pwm;
}

// PWM値の取得
// -> PWM値
int MotorDriver::getPWM(){
  return pwm;
}


