/**
  カラーLEDのクラス
  author: Yu Iijima
 **/

#include "ColorLED.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// コンストラクタ
// pin: カラーLEDと接続しているArduinoのPIN番号
// num: 接続しているカラーLEDの数
ColorLED::ColorLED(int pin, int num)
  :
  led(Adafruit_NeoPixel(num, pin, NEO_RGB + NEO_KHZ800)),
  number(num)
{
  led.begin();
  switchOff();
}

// カラーLEDを光らせる
// r: RGBのR(0-255)
// g: RGBのG(0-255)
// b: RGBのB(0-255)
// brightness: 光の強さ(0-255)
void ColorLED::shine(unsigned char r, unsigned char g, unsigned char b, unsigned char brightness) {
  led.setBrightness(brightness);
  for (int i = 0; i < number; i++) {
    led.setPixelColor(i, r, g, b);
  }
  led.show();
}

// カラーLEDを消灯する
void ColorLED::switchOff() {
  this->shine(0, 0, 0, 0);
}

