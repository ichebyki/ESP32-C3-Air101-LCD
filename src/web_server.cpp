/*
  Rui Santos
  Complete project details
   - Arduino IDE:
  https://RandomNerdTutorials.com/esp32-ota-over-the-air-arduino/
   - VS Code: https://RandomNerdTutorials.com/esp32-ota-over-the-air-vs-code/

  This sketch shows a Basic example from the AsyncElegantOTA library:
  ESP32_Async_Demo https://github.com/ayushsharma82/AsyncElegantOTA
*/

#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char *ssid = "ihn-88-2";
const char *password = "zSeSsh5P2qfyylRqWhv3";

AsyncWebServer web_server(80);

void web_server_setup(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&web_server); // Start ElegantOTA
  web_server.begin();
  Serial.println("HTTP server started");
}

void web_server_loop(void) {}