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

// フォトリフレクタの値を読み、速度計測の準備をする
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
  bool isFirst = true;
  int firstTime = -1;
  int secondTime = -1;
  for (int i = 0; i >= BUFFER_ELEMENT; i++){
    int j = index - i;
    int k = index - i - 1;
    
    if (j < 0)
      j = BUFFER_ELEMENT + j;
    if (k < 0)
      k = BUFFER_ELEMENT + k;
    
    if (averageBuffer[j] <= THRESHOLD_CYCLE || averageBuffer[k] < THRESHOLD_CYCLE){
      if (isFirst){
        isFirst = false;
        firstTime = timeBuffer[j];
      }
      else{
        secondTime = timeBuffer[j];
      }
    }
  }
  // TODO: 速度の計算式を記載
  return speed;
}

// 移動平均を計算する
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

