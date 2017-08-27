#include <SoftwareSerial.h>

SoftwareSerial rn4020(2, 3);

struct commandTable {
  char comm[50];
  int len;
};

struct commandTable reboot = {"R,1",3};
struct commandTable baudRate = {"SB,1",4}; //baud rate 9600

char recvChar;
String recvStr;

void setup() {
  // put your setup code here, to run once:
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
  recvStr = "";
  while(Serial.available()){
    recvChar = Serial.read();
    recvStr += recvChar;
    delay(10);
  }

  // 文字列の送信
  if (recvStr.compareTo("") != 0){
    Serial.print(recvStr);
    sendRN4020(recvStr, recvStr.length() - 2);
  }

  // 受信データの読込
  while(rn4020.available()){
    Serial.print((char)rn4020.read());
  }
}

void sendRN4020(String s,int l){
  for(int i=0;i<l;i++){
    rn4020.write(s.charAt(i));
    delay(20);
  }
  rn4020.write('\n');
}

