#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

const int PIN_IN1_R = 5; // 右モータ１のPIN番号
const int PIN_IN2_R = 6; // 右モータ２のPIN番号
const int PIN_IN1_L = 9; // 左モータ１のPIN番号
const int PIN_IN2_L = 10; // 左モータ２のPIN番号
const int PIN_RGBLED = 16; // カラーLEDのPIN番号
const int NUM_RGBLED = 2; // カラーLEDの数
const String LEFT_MOTOR_HANDLE = "0018"; // 左モータのハンドル名
const String RIGHT_MOTOR_HANDLE = "001B"; // 右モータのハンドル名
const String RGBLED_HANDLE = "001E"; // 左モータのハンドル名
const String MOTOR_HANDLE = "0021"; // モータのハンドル名

SoftwareSerial rn4020(2, 3);
Adafruit_NeoPixel RGBLED = Adafruit_NeoPixel(NUM_RGBLED, PIN_RGBLED, NEO_RGB + NEO_KHZ800);

struct commandTable {
  char comm[50];
  int len;
};

enum Motor {
  Right,
  Left,
};

struct commandTable reboot = {"R,1",3};
struct commandTable baudRate = {"SB,1",4}; //baud rate 9600

String inputStr; // シリアルモニタからの入力文字列
String readStr; // RN4020から受信した文字列

String recvBuffer; // 受信用のバッファ

void setup() {
  // カラーLEDの初期化
  RGBLED.begin();
  shineColorLED(0,0,0,0);

  // PINの設定
  pinMode(PIN_IN1_R,OUTPUT); 
  pinMode(PIN_IN2_R,OUTPUT);
  pinMode(PIN_IN1_L,OUTPUT); 
  pinMode(PIN_IN2_L,OUTPUT); 

  // グローバル変数の初期化
  recvBuffer = "";

  // シリアル通信の初期化
  Serial.begin(57600);
  delay(100);
  Serial.setTimeout(10);
  
  // RN4020の初期設定(必要な時だけ実施)
  /*
  rn4020.begin(115200);
  delay(500);
  sendRN4020(baudRate.comm, baudRate.len);
  delay(100);
  sendRN4020(reboot.comm, reboot.len);
  delay(2000);
  */
  
  // RN4020の初期化
  rn4020.begin(9600);
  delay(100);

  // キャラクタリスティックの初期化
  sendRN4020("SHW,"+LEFT_MOTOR_HANDLE+",30",11);
  sendRN4020("SHW,"+RIGHT_MOTOR_HANDLE+",30",11);
  sendRN4020("SHW,"+RGBLED_HANDLE+",00000000",17);
  sendRN4020("SHW,"+MOTOR_HANDLE+",0000",13);
}

void loop() {
  // シリアルモニタから文字列入力
  inputStr = "";
  char inputChar;
  while(Serial.available()){
    inputChar = Serial.read();
    inputStr += inputChar;
    delay(10);
  }

  // 文字列の送信
  if (inputStr.compareTo("") != 0){
    Serial.print(inputStr);
    sendRN4020(inputStr, inputStr.length() - 2);
  }

  // 受信データの読込
  while(rn4020.available()){
    recvBuffer += (char)rn4020.read();
  }

  analyseBuffer();
}

// 受信データの解析
void analyseBuffer(){
  int eol = -1;
  String line = "";
  while ((eol = recvBuffer.indexOf("\r\n")) >= 0){
    line = recvBuffer.substring(0, eol);
    analyseLine(line);

    if(eol == recvBuffer.length()){
      recvBuffer = "";
      break;
    }
    recvBuffer = recvBuffer.substring(eol + 2);
  }
}

// 1行単位での解析
void analyseLine(String line){
  Serial.println(line);
  if(line.startsWith("WV,")){
    String residual = line.substring(line.indexOf(',') + 1);
    if (residual.startsWith(LEFT_MOTOR_HANDLE)){
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      int value = convertStr2Int(residual);
      Serial.println(value);
      rotateMotor(value, Left);
    }
    else if (residual.startsWith(RIGHT_MOTOR_HANDLE)){
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      int value = convertStr2Int(residual);
      Serial.println(value);
      rotateMotor(value, Right);
    }
    else if (residual.startsWith(MOTOR_HANDLE)){
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      char left = convertStr2Char(residual.substring(0,2));
      char right = convertStr2Char(residual.substring(2,4));
      Serial.println(left,DEC);
      Serial.println(right,DEC);
      rotateMotor((int)left,Left);
      rotateMotor((int)right,Right);
    }
    else if (residual.startsWith(RGBLED_HANDLE)){
      residual = residual.substring(residual.indexOf(',') + 1, residual.length() - 1);
      unsigned char r = convertStr2UChar(residual.substring(0,2));
      unsigned char g = convertStr2UChar(residual.substring(2,4));
      unsigned char b = convertStr2UChar(residual.substring(4,6));
      unsigned char brgt = convertStr2UChar(residual.substring(6,8));
      Serial.println(r,DEC);
      Serial.println(g,DEC);
      Serial.println(b,DEC);
      Serial.println(brgt,DEC);
      shineColorLED(r,g,b,brgt);
    }
  }
}

// カラーLEDを光らせる
void shineColorLED(unsigned char r, unsigned char g, unsigned char b, unsigned char brightness){
  RGBLED.setBrightness(brightness);
  for (int i = 0; i < NUM_RGBLED; i++){
    RGBLED.setPixelColor(i, r, g, b);
  }
  RGBLED.show();
}

// 文字列から整数型の変換
int convertStr2Int(String str){
  int value = 0;
  int sign = 1;
  if (str.substring(0,2).compareTo("2D") == 0)
    sign = -1;
    
  for(int i = 0; i < str.length(); i+=2){
    if (str.charAt(i) == '3'){
      int num = str.charAt(i+1) - '0';
      value = value * 10 + num;
    }
  }
  value = sign * value;
  return value;
}

// 文字列からChar型への変換
char convertStr2Char(String str){
  int value = 0;
  value = str.charAt(0) * 16 + str.charAt(1);
  value -= 128;
  return (char)value;
}

// 文字列からunsined char型への変換
unsigned char convertStr2UChar(String str){
  int value = 0;
  value = str.charAt(0) * 16 + str.charAt(1);
  return (unsigned char)value;
}

// モータの回転
void rotateMotor(int pwm, Motor m){
  boolean isAhead = (pwm > 0);
  boolean isBreak = (pwm == 0);
  
  int val = (int)(abs(pwm)/100.0*255);
  
  Serial.print(val);
  Serial.print(",");
  Serial.print(isAhead);
  Serial.print(",");
  Serial.print(isBreak);
  Serial.print(",");
  Serial.println(m);

  if (m == Right){
    if (isBreak){
      digitalWrite(PIN_IN1_R, HIGH);
      digitalWrite(PIN_IN2_R, HIGH);      
    }
    else if (isAhead){
      analogWrite(PIN_IN1_R, 0);
      analogWrite(PIN_IN2_R, val);
    }
    else{
      analogWrite(PIN_IN1_R, val);
      analogWrite(PIN_IN2_R, 0);
    }
  }
  else{
    if (isBreak){
      digitalWrite(PIN_IN1_L, HIGH);
      digitalWrite(PIN_IN2_L, HIGH);      
    }
    else if (isAhead){
      analogWrite(PIN_IN1_L, 0);
      analogWrite(PIN_IN2_L, val);
    }
    else{
      analogWrite(PIN_IN1_L, val);
      analogWrite(PIN_IN2_L, 0);
    }
  }
}

// RN4020への文字列送信
void sendRN4020(String s,int l){
  for(int i=0;i<l;i++){
    rn4020.write(s.charAt(i));
    delay(20);
  }
  rn4020.write('\n');
}

