/**
  Bluetooth通信モジュール RN4020 のクラス
  author: Yu Iijima
 **/

#include "RN4020.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

// コンストラクタ
// pinRx: RN4020のRXと接続しているArduinoのPIN番号
// pinTx: RN4020のTXと接続しているArduinoのPIN番号
// baurate: ボーレート
RN4020::RN4020(int pinRx, int pinTx, int baudRate)
:
  serial(SoftwareSerial(pinTx, pinRx)),
  baudRate(baudRate)
{
}

// RN4020の開始準備
void RN4020::begin(){
  serial.begin(baudRate);
  delay(100);
}

// RN4020への文字列送信
// str: 送信したい文字列
void RN4020::send(String str){
  for (int i = 0; i < str.length(); i++) {
    serial.write(str.charAt(i));
    delay(20);
  }
  serial.write('\n');
}

// RN4020からの受信
// -> 受信文字列
String RN4020::receive(){
  String recvBuffer = "";
  while (serial.available()) {
    recvBuffer += (char)serial.read();
  }
  return recvBuffer;
}

// RN4020のリブート
void RN4020::reboot(){
  this->send("R,1");
  delay(2000);
}

// ボーレートの変更
void RN4020::changeBaudrate(int baudRate){
  this->baudRate = baudRate;
  int set = 1;
  switch (baudRate){
  case 2400:
    set = 0;
    break;
  case 9600:
    set = 1;
    break;
  case 19200:
    set = 2;
    break;
  case 38400:
    set = 3;
    break;
  case 115200:
    set = 4;
    break;
  case 230400:
    set = 5;
    break;
  case 460800:
    set = 6;
    break;
  case 921600:
    set = 7;
    break;
  }
  this->send("SB," + set);
  this->begin();
}

