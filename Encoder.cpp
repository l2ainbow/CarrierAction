/**
  エンコーダクラス
  author: Yu Iijima
 **/

#include "Encoder.h"
#include <Arduino.h>

// コンストラクタ
// pin: エンコーダと接続しているArduinoのPIN番号
Encoder::Encoder(int pin)
:
pin(pin)
{
}

void Encoder::encode() {
  int index = cnt % BUFFER_ELEMENT;
  valueBuffer[index] = analogRead(pin);
  timeBuffer[index] = micros();
  averageBuffer[index] = movingAverage(AVERAGING_TIME);
  cnt++;
}

// 速度を取得する
float Encoder::getSpeed() {
  float speed = 0.0;
  if (cnt < 1){
    return speed;
  }
  int index = cnt % BUFFER_ELEMENT;
  while(averageBuffer[index] >= THRESHOLD_CYCLE || averageBuffer[(index - 1) % BUFFER_ELEMENT] < THRESHOLD_CYCLE){

    index -- ;
  }
  return speed;
}

int Encoder::movingAverage(int time) {
  int average = 0;
  int sum = 0;
  int i;
  for (i = 0; i< time; i++){
    if (cnt < i){
      break;
    }
    sum += valueBuffer[(cnt - i) % BUFFER_ELEMENT];
  }
  average = sum / (i + 1);
  return average;
}

