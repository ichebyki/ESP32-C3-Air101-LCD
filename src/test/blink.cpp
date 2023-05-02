#include "options.h"
#ifndef NO_BLINK
#include <Arduino.h>

uint8_t blink = 0;
time_t blink_time;

void blink_setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  // log_e(">> Serial baudrate: %d\n", Serial.baudRate());
  // log_e(">> Serial available: %d\n", Serial.available());
  blink = millis();
  Serial.println("Blink setup completed");

  blink = millis();
}

void blink_loop() {
  if (millis() - blink_time > 7000) {
    blink_time = millis();
    digitalWrite(12, blink % 2);
    blink++;
    digitalWrite(13, blink % 2);
    delay(100);
    digitalWrite(12, blink % 2);
    blink++;
    digitalWrite(13, blink % 2);
    delay(100);
    digitalWrite(12, blink % 2);
    blink++;
    digitalWrite(13, blink % 2);
  }
}
#endif
