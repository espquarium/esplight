// todo https://stackoverflow.com/questions/2548075/c-string-template-library
#include "Arduino.h"
#include "DNSServer.h"
#include "WebServer.h"
#include "WiFi.h"
#include "WiFiManager.h"
#include "WiFiUdp.h"
#include "date_struct.h"
#include "esp_adc_cal.h"
#include "light_helper.h"
#include "ntp_helper.h"
#include "storage_helper.h"
#include "time.h"

#define EEPROM_SIZE 1

void espDelay(int ms) {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

WiFiManager wifiManager;

LighTimeStorage lightStorage = LighTimeStorage();
WebServer server(80);
LightHelper light = LightHelper();
NTPHelper ntpHelper = NTPHelper();

void saveConfigCallback() {
    Serial.println("Configuração salva");
}

void connectToWifi() {
    String portalName = "ESPLIGHT-WIFI";

    wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 0, 98), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));

    while (!wifiManager.autoConnect(portalName.c_str())) {
        Serial.println("retrying wifi conection");
        espDelay(3000);
    }
}

void setCrossOrigin() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

void handleNotFound() {
    if (server.method() == HTTP_OPTIONS) {
        setCrossOrigin();
        server.send(204);
    } else {
        server.send(404, "text/plain", "");
    }
}

void sendCrossOriginHeader() {
    Serial.println(F("sendCORSHeader"));
    setCrossOrigin();
    server.send(204);
}

void handleToggle() {
    light.setForceLight(!light.forceLight);
    setCrossOrigin();
    server.send(200, "text/plain", "toggled light");
}

void handleVerify() {
    // String resText = "light is ";
    // resText += light.forceLight ? "on" : "off";
    String resText = "{\"force\":";
    resText += (light.forceLight ? "true" : "false");
    resText += "}";
    setCrossOrigin();
    server.send(200, "application/json", resText);
}

void handleHour() {
    // String resText = "light is ";
    // resText += light.forceLight ? "on" : "off";
    Date time = ntpHelper.getTime();
    String resText = String(time.h);
    resText += ":";
    resText += String(time.m);
    setCrossOrigin();
    server.send(200, "text/plain", resText);
}

void handleLightTimes() {
    setCrossOrigin();
    server.send(200, "application/json", lightStorage.getTimesAsJson());
}

void handleUpdateLightTimes() {
    lightStorage.save(server.arg(0));
    setCrossOrigin();
    server.send(200, "application/json", lightStorage.getTimesAsJson());
}

void setup() {
    light.setup();

    Serial.begin(9600);
    // Serial.write(27);  // Print "esc" Serial.print("[2J");
    Serial.println("Start");

    lightStorage.setup();
    lightStorage.load();

    wifiManager.setConfigPortalTimeout(180);

    connectToWifi();

    server.on("/", HTTP_GET,
              []() {
                  Serial.println(lightStorage.timesSaved);
                  server.send(200, "text/plain", "Welcome to reeflight :)");
              });

    server.on("/toggle", HTTP_GET,
              handleToggle);

    server.on("/verify", HTTP_GET,
              handleVerify);

    server.on("/hour", HTTP_GET,
              handleHour);

    server.on("/light-times", HTTP_GET,
              handleLightTimes);

    server.on("/light-times", HTTP_OPTIONS, sendCrossOriginHeader);

    server.on("/light-times", HTTP_PUT,
              handleUpdateLightTimes);

    server.onNotFound(handleNotFound);

    server.begin();
}

void loop() {
    Date timeNow = ntpHelper.getTime();

    light.loop(timeNow, lightStorage.timesSaved, lightStorage.lightTimes);

    server.handleClient();
}
