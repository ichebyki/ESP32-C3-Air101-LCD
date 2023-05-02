#include <Adafruit_GFX.h>    // графическая библиотека
#include <Adafruit_ST7735.h> // библиотека для ST7735
#include <Arduino.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>

#include "air101_lcd.h"
#include "options.h"

#define sclk 2
#define mosi 3
#define miso 12
#define cs 7
#define dc 6
#define rst 10

// Option 1: use any pins but a little slower
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
uint8_t rotation = 3;
uint16_t current_freq = 1052;

const uint8_t BL_PIN = 11;
const uint8_t KEY_PINS[5] = {8, 9, 13, 5, 4}; // UP, RT, DN, LT, CR

void joystick_loop() {
  static uint8_t keys = 0x1F;

  // Serial.printf("Button: %d, %d, %d, %d, %d\n", digitalRead(KEY_PINS[0]),
  //                digitalRead(KEY_PINS[1]), digitalRead(KEY_PINS[2]),
  //                digitalRead(KEY_PINS[3]), digitalRead(KEY_PINS[4]));
  for (uint8_t i = 0; i < sizeof(KEY_PINS) / sizeof(KEY_PINS[0]); ++i) {
    if (((keys >> i) & 0x01) != digitalRead(KEY_PINS[i])) {
      if ((keys >> i) & 0x01) {
        switch (i) {
        case 0: // UP
          break;
        case 1: // RT
          if (current_freq < 1500)
            ++current_freq;
          break;
        case 2: // DN
          break;
        case 3: // LT
          if (current_freq > 500)
            --current_freq;
          break;
        case 4: // CR
          rotation++;
          rotation = rotation % 4;
          break;
        default:
          break;
        }
      }
      keys ^= (1 << i);
    }
  }
}

void tft_setup() {
  for (uint8_t i = 0; i < sizeof(KEY_PINS) / sizeof(KEY_PINS[0]); ++i) {
    pinMode(KEY_PINS[i], INPUT_PULLUP);
  }

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BL_PIN, 0);
  // ledcWrite(0, 127);
  delay(1000);

  SPI.begin(2, 12, 3, 7);
  Serial.println();

  tft.initR(INITR_MINI160x80_PLUGIN); // initialize a ST7735S chip
  // Serial.println ("TFT initialized");
  tft.setRotation(rotation);

  // prepare the TFT
  tft.setTextColor(ST7735_WHITE);
  tft.fillScreen(ST7735_BLACK);

  tft.drawRect(0, 0, 160, 80,
               ST7735_YELLOW);                             // upper rectangle

  tft.setCursor(30, 15); // put text in upper rectangle
  tft.setTextSize(2);    // select text size
  tft.println("ST7735");
  tft.setCursor(30, 30);
  tft.println("======");
  tft.setTextSize(1); // select text size
  tft.setCursor(30, 45);
  tft.println("160x80 pixels");
}

void show_freq(uint16_t new_freq, const char *suffix) {
  static uint16_t old_freq = 0;
  static char curr[8] = {'1', '2', '3', '.', '5', 0, 0, 0};

  if (old_freq != new_freq) {
    int16_t x1, y1;
    uint16_t w1, h1;
    uint16_t w2, h2;

    old_freq = new_freq;
    snprintf(curr, 8, "%d.%d", new_freq / 10, new_freq - (new_freq / 10) * 10);

    tft.setFont(&FreeMonoBold18pt7b);
    tft.getTextBounds(curr, 0, 20, &x1, &y1, &w1, &h1);
    tft.setFont(&FreeMono9pt7b);
    tft.getTextBounds(suffix, 0, 20, &x1, &y1, &w2, &h2);

    tft.setTextColor(ST7735_YELLOW);
    tft.fillRect(1, 1, 158, 78, ST7735_BLACK);

    tft.setTextSize(1); // select text size
    tft.setFont(&FreeMonoBold18pt7b);
    tft.setCursor((160 - w1 - w2) / 2, 5 + h1);
    tft.print(curr);

    tft.setFont(&FreeMono9pt7b);
    tft.setCursor((160 - w1 - w2) / 2 + w1, 5 + h1);
    tft.print(suffix);
  }
}

void tft_loop() {
  show_freq(current_freq, " MHz");
  // tft.setTextSize(1); // select text size
  // tft.setCursor(3, 80 - 15);
  // tft.print("Hello");
  //delay(500);
  // tft.fillRect(3, 80 - 15, 60, 15,
  //              ST7735_RED); // clear "Hello"
  // tft.setCursor(3, 80 - 15);
  // tft.print("World");
  //delay(500);
  // tft.fillRect(3, 80 - 15, 60, 15,
  //              ST7735_RED); // clear "World"
}
