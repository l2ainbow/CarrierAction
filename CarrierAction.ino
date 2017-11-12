#include "MotorDriver.h"
#include "Encoder.h"

/*** 定数 ***/
// RN4020のRXのPIN番号
const int PIN_RX_RN4020 = 3;
// RN4020のTXのPIN番号
const int PIN_TX_RN4020 = 2;
// RN4020のボーレート
const int BAUD_RATE = 9600;
// 右モータ１のPIN番号
const int PIN_IN1_R = 5;
// 右モータ２のPIN番号
const int PIN_IN2_R = 6;
// 左モータ１のPIN番号
const int PIN_IN1_L = 9;
// 左モータ２のPIN番号
const int PIN_IN2_L = 10;
// カラーLEDのPIN番号
const int PIN_RGBLED = 16;
const int PIN_ENCODER_R = 17;
const int PIN_ENCODER_L = 15; 
// カラーLEDの数
const int NUM_RGBLED = 4;
// モータのブースト時間（停止〜加速）[ms]
const int BOOST_TIME = 50;
// 接続時の色
const int CONNECTED_RGB[] = {0, 255, 0};
// 未接続時の色
const int DISCONNECTED_RGB[] = {255, 0, 0};
// 左モータのハンドル名
const String LEFT_MOTOR_HANDLE = "0018";
// 右モータのハンドル名
const String RIGHT_MOTOR_HANDLE = "001B";
// カラーLEDのハンドル名
const String RGBLED_HANDLE = "001E";
// モータのハンドル名
const String MOTOR_HANDLE = "0021";

/*** グローバル変数 ***/
// 右モータドライバのインスタンス
MotorDriver rightMotorDriver = MotorDriver(PIN_IN1_R, PIN_IN2_R);
// 左モータドライバのインスタンス
MotorDriver leftMotorDriver = MotorDriver(PIN_IN1_L, PIN_IN2_L);
// 右エンコーダのインスタンス
Encoder rightEncoder = Encoder(PIN_ENCODER_R);

// RN4020より受信した文字列のバッファ
String recvBuffer;
// BLEで接続しているか
bool isConnected;

// 右モータのPWM
volatile int rightPWM;
// 左モータのPWM
volatile int leftPWM;

/*** 関数 ***/

// Arduino起動時の処理
void setup() {
  
  // グローバル変数の初期化
  recvBuffer = "";
  isConnected = false;

  rightMotorDriver.rotate(100);

  Serial.begin(57600);
}

int cnt = 0;

// Arduinoのループ処理
void loop() {
  // シリアルモニタから文字列入力
  String inputStr = "";
  char inputChar;
  int val = analogRead(PIN_ENCODER_L);
  cnt++;
  
  rightEncoder.encode();
  
  if (cnt % 100 == 0){
    float speed = rightEncoder.getSpeed();
    Serial.println(String(millis())+",CarrierAction,speed,"+String(speed));
  }
}


