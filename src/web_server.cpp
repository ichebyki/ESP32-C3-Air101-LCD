#include <ESPmDNS.h>
#include <Update.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>
#include "SPIFFS.h"
#include "options.h"
#include "web_server.h"

const char *hostname = "esp32-c3-air101-lcd";
const char *ssid = "xxxxxxxx";
const char *password = "yyyyyyyy";

WiFiMulti wifiMulti;
WebServer server(80);

/***********************************************************************************************************************
 *                                         C O N N E C T   TO   W I F I *
 ***********************************************************************************************************************/
bool connectToWiFi() {
  String s_ssid = "", s_password = "", s_info = "";
  wifiMulti.addAP(ssid, password);     // SSID and PW in code
  WiFi.setHostname(hostname);
  if (psramFound())
    WiFi.useStaticBuffers(true);
  File file = SPIFFS.open("/networks.csv",
                          "r"); // try credentials given in "/networks.txt"
  if (file) {
    String str = "";
    while (file.available()) {
      str = file.readStringUntil('\n'); // read the line
      if (str[0] == '*')
        continue; // ignore this, goto next line
      if (str[0] == '\n')
        continue; // empty line
      if (str[0] == ' ')
        continue; // space as first char
      if (str.indexOf('\t') < 0)
        continue; // no tab
      str += "\t";
      uint p = 0, q = 0;
      s_ssid = "", s_password = "", s_info = "";
      for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\t') {
          if (p == 0)
            s_ssid = str.substring(q, i);
          if (p == 1)
            s_password = str.substring(q, i);
          if (p == 2)
            s_info = str.substring(q, i);
          p++;
          i++;
          q = i;
        }
      }
      // log_i("s_ssid=%s  s_password=%s  s_info=%s", s_ssid.c_str(),
      // s_password.c_str(), s_info.c_str());
      if (s_ssid == "")
        continue;
      if (s_password == "")
        continue;
      wifiMulti.addAP(s_ssid.c_str(), s_password.c_str());
    }
    file.close();
  }

  // WiFi.begin(ssid, password);
  wifiMulti.run();

  if (WiFi.isConnected()) {
    // Connected to WiFi network
    Serial.println("Connected to WiFi");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.setSleep(false);
    return true;
  }

  Serial.println("WiFi credentials are not correct or other problem\n");
  return false; // can't connect to any network
}

void sendFile(const char *path, const String &contentType) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = SPIFFS.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }
  size_t sent = server.streamFile(file, contentType);
  file.close();
}

/*
 * setup function
 */
void web_server_setup(void) {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to the best WiFi
  if (!connectToWiFi()) {
    Serial.println("WiFi credentials are not correct");
    return;
  }

  /*use mdns for host name resolution*/
  if (!MDNS.begin(hostname)) { // http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in otaUpdatePage */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    sendFile("/loginIndex.html", "text/html");
  });
  server.on("/ota-update", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    sendFile("/otaUpdatePage.html", "text/html");
  });
  /*handling uploading firmware file */
  server.on(
      "/update", HTTP_POST,
      []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
      },
      []() {
        HTTPUpload &upload = server.upload();
        int upload_type = U_FLASH;
        if (upload.status == UPLOAD_FILE_START) {
          Serial.printf("upload_type=%s\n", server.arg("upload_type"));
          if (server.hasArg("upload_type")) {
            String uptype = server.arg("upload_type");
            if (uptype.equals("spiffs")) {
              upload_type = U_SPIFFS;
            }
          }
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                            upload_type)) { // start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) !=
              upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) { // true to set the size to the current
                                  // progress
            Serial.printf("Update Success: %u\nRebooting...\n",
                          upload.totalSize);
          } else {
            Update.printError(Serial);
          }
        }
      });
  server.begin();
}

void web_server_loop(void) {
  server.handleClient();
}