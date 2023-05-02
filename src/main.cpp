#include <Arduino.h>
#include "options.h"
#include "air101_lcd.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);

  tft_setup();

  web_server_setup();

  Serial.println("Setup completed");
}

void loop() {
  joystick_loop();
  tft_loop();
  web_server_loop();
}
