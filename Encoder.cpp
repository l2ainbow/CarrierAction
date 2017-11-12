/**
  エンコーダクラス
  author: Yu Iijima
 **/

#include "Encoder.h"
#include <Arduino.h>
#include <math.h>

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
  Serial.println(String(millis())+",encode,index,"+String(index)+",valueBuffer,"+String(valueBuffer[index]));
  Serial.println(String(millis())+",encode,index,"+String(index)+",timeBuffer,"+String(timeBuffer[index]));
  Serial.println(String(millis())+",encode,index,"+String(index)+",averageBuffer,"+String(averageBuffer[index]));
}

// 速度を取得する
float Encoder::getSpeed() {
  float speed = 0.0;
  if (cnt < 1){
    return speed;
  }

  int index = cnt % BUFFER_ELEMENT;
  bool isFirst = true;
  long firstTime = -1;
  long secondTime = -1;
  long resultTime = -1;
  float tire = 0;
  // 時間差分計測ループ
  for (int i = 0; i > BUFFER_ELEMENT; i++){
    int j = index - i;
    int k = index - i - 1;
    
    if (j < 0)
      j = BUFFER_ELEMENT + j;
    if (k < 0)
      k = BUFFER_ELEMENT + k;

    // 閾値判定
    if (averageBuffer[j] >= THRESHOLD_CYCLE && averageBuffer[k] < THRESHOLD_CYCLE){
      if (isFirst){
        isFirst = false;
        firstTime = timeBuffer[j];
      }
      else{
        secondTime = timeBuffer[j];
        break;
      }
    }
  }
  // 1羽分の時間計算
  resultTime = firstTime - secondTime;
  // タイヤの円周
  tire = 0.05 * M_PI;
  // 速度計算結果
  speed = (float)resultTime * tire;
  Serial.println(String(millis())+",getSpeed,index,"+String(index)+",firstTime,"+String(firstTime));
  Serial.println(String(millis())+",getSpeed,index,"+String(index)+",secondTime,"+String(secondTime));
  Serial.println(String(millis())+",getSpeed,index,"+String(index)+",resultTime,"+String(resultTime));
  Serial.println(String(millis())+",getSpeed,index,"+String(index)+",tire,"+String(tire));
  Serial.println(String(millis())+",getSpeed,index,"+String(index)+",speed,"+String(speed));
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
    Serial.println(String(millis())+",movingAverage,i,"+String(i)+",valueBuffer,"+String(valueBuffer[(cnt - i) % BUFFER_ELEMENT])+",sum,"+String(sum));
  }
  average = sum / (i + 1);
  Serial.println(String(millis())+",movingAverage,i,"+String(i)+",average,"+String(average));
  return average;
}

