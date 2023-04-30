
#include <Arduino.h>
#include "options.h"
#include "air101_lcd.h"
#include "blink.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);

  air101_lcd_setup();
  web_server_setup();
  //blink_setup();

  Serial.println("Setup completed");
}

void loop() {
  keys_loop();
  web_server_loop();
  //blink_loop();
}
