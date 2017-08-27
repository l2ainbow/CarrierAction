#include <SoftwareSerial.h>

#define PIN_IN1_R  4
#define PIN_IN2_R  5
#define PIN_VREF_R 6 // PWM
#define PIN_IN1_L  7
#define PIN_IN2_L  8
#define PIN_VREF_L 9 // PWM

const String LEFT_MOTOR_HANDLE = "0018"; // 左モータのハンドル名
const String RIGHT_MOTOR_HANDLE = "001B"; // 右モータのハンドル名

SoftwareSerial rn4020(2, 3);

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

int leftMotorPWM; // 左モータのPWM(-100~100)
int rightMotorPWM; // 右モータのPWM(-100~100)

void setup() {
  pinMode(PIN_IN1_R,OUTPUT); 
  pinMode(PIN_IN2_R,OUTPUT);
  pinMode(PIN_IN1_L,OUTPUT); 
  pinMode(PIN_IN2_L,OUTPUT); 
  
  recvBuffer = "";
  leftMotorPWM = 0;
  rightMotorPWM = 0;
  
  Serial.begin(57600);
  delay(100);
  
  /* First Setting */
  rn4020.begin(115200);
  delay(500);
  sendRN4020(baudRate.comm, baudRate.len);
  delay(100);
  sendRN4020(reboot.comm, reboot.len);
  delay(2000);
  /* First Setting */
  
  rn4020.begin(9600);
  delay(100);

  Serial.setTimeout(10);
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
  }
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

// モータの回転
void rotateMotor(int pwm, Motor m){
  boolean isAhead = (pwm > 0);
  boolean isBreak = (pwm == 0);
  
  int val = (int)(abs(pwm)/100.0*255);
  
  Serial.print(val);
  Serial.print(",");
  Serial.print(isAhead);
  Serial.print(",");
  Serial.println(isBreak);
  
  if (m == Right){
    analogWrite(PIN_VREF_R, val);
    if (isBreak){
      digitalWrite(PIN_IN1_R, HIGH);
      digitalWrite(PIN_IN2_R, HIGH);      
    }
    else if (isAhead){
      digitalWrite(PIN_IN1_R, HIGH);
      digitalWrite(PIN_IN2_R, LOW);      
    }
    else{
      digitalWrite(PIN_IN1_R, LOW);
      digitalWrite(PIN_IN2_R, HIGH);      
    }
  }
  else{
    analogWrite(PIN_VREF_L, val);
    if (isBreak){
      digitalWrite(PIN_IN1_L, HIGH);
      digitalWrite(PIN_IN2_L, HIGH);      
    }
    else if (isAhead){
      digitalWrite(PIN_IN1_L, HIGH);
      digitalWrite(PIN_IN2_L, LOW);      
    }
    else{
      digitalWrite(PIN_IN1_L, LOW);
      digitalWrite(PIN_IN2_L, HIGH);      
    }
  }
  delay(100);
}

// RN4020への文字列送信
void sendRN4020(String s,int l){
  for(int i=0;i<l;i++){
    rn4020.write(s.charAt(i));
    delay(20);
  }
  rn4020.write('\n');
}

