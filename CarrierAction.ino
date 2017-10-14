#include <MsTimer2.h>
#include <SoftwareSerial.h>
#include "ColorLED.h"
#include "MotorDriver.h"

/*** 構造体 ***/
// RN4020用コマンドテーブル
// comm: コマンド
// len: コマンドの文字長
struct commandTable {
  char comm[50];
  int len;
};

/*** 定数 ***/
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
// カラーLEDの数
const int NUM_RGBLED = 2;
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
// リブート用のコマンド
const commandTable REBOOT = {"R,1", 3};
// ボーレート変更のためのコマンド
const commandTable BAUDRATE = {"SB,1", 4};

/*** グローバル変数 ***/
// RN4020のインスタンス
SoftwareSerial rn4020(2, 3);
// カラーLEDのインスタンス
ColorLED colorLED = ColorLED(PIN_RGBLED, NUM_RGBLED);
// 右モータドライバのインスタンス
MotorDriver rightMotorDriver = MotorDriver(PIN_IN1_R, PIN_IN2_R);
// 左モータドライバのインスタンス
MotorDriver leftMotorDriver = MotorDriver(PIN_IN1_L, PIN_IN2_L);

// シリアルモニタからの入力文字列
String inputStr;
// RN4020から受信した文字列
String readStr;

// BLE受信用のバッファ
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

  // シリアル通信の初期化
  Serial.begin(57600);
  delay(100);
  Serial.setTimeout(10);

  // RN4020の初期設定(必要な時だけ実施)
  /*
    rn4020.begin(115200);
    delay(500);
    sendRN4020(BAUDRATE.comm, BAUDRATE.len);
    delay(100);
    sendRN4020(REBOOT.comm, REBOOT.len);
    delay(2000);
  */

  // RN4020の初期化
  rn4020.begin(9600);
  delay(100);

  // キャラクタリスティックの初期化
  sendRN4020("SHW," + LEFT_MOTOR_HANDLE + ",30", 11);
  sendRN4020("SHW," + RIGHT_MOTOR_HANDLE + ",30", 11);
  sendRN4020("SHW," + RGBLED_HANDLE + ",00000000", 17);
  sendRN4020("SHW," + MOTOR_HANDLE + ",0000", 13);

  // カラーLEDを未接続状態の色に変更
  colorLED.shine(DISCONNECTED_RGB[0], DISCONNECTED_RGB[1], DISCONNECTED_RGB[2], 255);
}

// Arduinoのループ処理
void loop() {
  // シリアルモニタから文字列入力
  inputStr = "";
  char inputChar;
  while (Serial.available()) {
    inputChar = Serial.read();
    inputStr += inputChar;
    delay(10);
  }

  // 文字列の送信
  if (inputStr.compareTo("") != 0) {
    Serial.print(inputStr);
    sendRN4020(inputStr, inputStr.length() - 2);
  }

  // 受信データの読込
  while (rn4020.available()) {
    recvBuffer += (char)rn4020.read();
  }

  // 受信データの解析
  analyseBuffer();
}

// 受信データの解析
void analyseBuffer() {
  int eol = -1;
  String line = "";
  while ((eol = recvBuffer.indexOf("\r\n")) >= 0) {
    line = recvBuffer.substring(0, eol);
    analyseLine(line);

    if (eol == recvBuffer.length()) {
      recvBuffer = "";
      break;
    }
    recvBuffer = recvBuffer.substring(eol + 2);
  }
}

// 1行単位での解析
// line: 1行分の文字列
void analyseLine(String line) {
  Serial.println(line);
  if (line.startsWith("WV,")) {
    String residual = line.substring(line.indexOf(',') + 1);
    if (residual.startsWith(LEFT_MOTOR_HANDLE)) {
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      int value = convertStr2Int(residual);
      Serial.println(value);
      rotateLeftMotor(value);
    }
    else if (residual.startsWith(RIGHT_MOTOR_HANDLE)) {
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      int value = convertStr2Int(residual);
      Serial.println(value);
      rotateRightMotor(value);
    }
    else if (residual.startsWith(MOTOR_HANDLE)) {
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      char left = convertStr2Char(residual.substring(0, 2));
      char right = convertStr2Char(residual.substring(2, 4));
      Serial.println(left, DEC);
      Serial.println(right, DEC);
      rotateLeftMotor((int)left);
      rotateRightMotor((int)right);
    }
    else if (residual.startsWith(RGBLED_HANDLE)) {
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      unsigned char r = convertStr2UChar(residual.substring(0, 2));
      unsigned char g = convertStr2UChar(residual.substring(2, 4));
      unsigned char b = convertStr2UChar(residual.substring(4, 6));
      //unsigned char brgt = convertStr2UChar(residual.substring(6,8));
      unsigned char brgt = 255;
      Serial.println(r, DEC);
      Serial.println(g, DEC);
      Serial.println(b, DEC);
      Serial.println(brgt, DEC);
      colorLED.shine(r, g, b, brgt);
    }
  }
  else if (line.startsWith("Connected")) {
    isConnected = true;
    colorLED.shine(CONNECTED_RGB[0], CONNECTED_RGB[1], CONNECTED_RGB[2], 255);
  }
  else if (line.startsWith("Connection End")) {
    isConnected = false;
    rotateRightMotor(0);
    rotateLeftMotor(0);
    colorLED.shine(DISCONNECTED_RGB[0], DISCONNECTED_RGB[1], DISCONNECTED_RGB[2], 255);
  }
}

// 文字列からint型の変換
// str: 変換したい文字列
// -> 変換後の数値(int型)
int convertStr2Int(String str) {
  int value = 0;
  int sign = 1;
  if (str.substring(0, 2).compareTo("2D") == 0)
    sign = -1;

  for (int i = 0; i < str.length(); i += 2) {
    if (str.charAt(i) == '3') {
      int num = str.charAt(i + 1) - '0';
      value = value * 10 + num;
    }
  }
  value = sign * value;
  return value;
}

// 文字列からChar型への変換
// str: 変換したい文字列
// -> 変換後の数値(char型)
char convertStr2Char(String str) {
  int value = 0;
  value = str.charAt(0) * 16 + str.charAt(1);
  value -= 128;
  return (char)value;
}

// 文字列からunsined char型への変換
// str: 変換したい文字列
// -> 変換後の数値(unsigned char型)
unsigned char convertStr2UChar(String str) {
  int value = 0;
  value = str.charAt(0) * 16 + str.charAt(1);
  return (unsigned char)value;
}

// 右モータの回転
// pwm: 右モータのPWM
void rotateRightMotor(int pwm) {
  int lastPWM = rightPWM;
  rightPWM = pwm;

  if (lastPWM == 0 && pwm != 0) {
    MsTimer2::set(BOOST_TIME, rotateRightMotorAgain);
    MsTimer2::start();
    rightMotorDriver.rotate(100);
  }
  else{
    rightMotorDriver.rotate(pwm);
  }
}

// 右モータの再回転
void rotateRightMotorAgain() {
  MsTimer2::stop();
  rightMotorDriver.rotate(rightPWM);
}

// 左モータの回転
// pwm: 左モータのPWM
void rotateLeftMotor(int pwm) {
  int lastPWM = leftPWM;
  leftPWM = pwm;
  
  if (lastPWM == 0 && pwm != 0) {
    MsTimer2::set(BOOST_TIME, rotateLeftMotorAgain);
    MsTimer2::start();
    leftMotorDriver.rotate(100);
  }
  else {
    leftMotorDriver.rotate(pwm);
  }
}

// 左モータの再回転
void rotateLeftMotorAgain() {
  MsTimer2::stop();
  leftMotorDriver.rotate(leftPWM);
}

// RN4020への文字列送信
// str: 送信したい文字列
// num: 送信したい文字数
void sendRN4020(String str, int num) {
  for (int i = 0; i < num; i++) {
    rn4020.write(str.charAt(i));
    delay(20);
  }
  rn4020.write('\n');
}

