#include <Arduino.h>
#include <TFT_eSPI.h>
#include "time.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

WiFiManager wifiManager;

AsyncWebServer server(80);

const char *ssid = "REPLACE_WITH_YOUR_SSID";
const char *password = "REPLACE_WITH_YOUR_PASSWORD";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

const char *PARAM_MESSAGE = "message";

void button_init()
{
  btn1.setLongClickHandler([](Button2 &b)
                           {
                             btnCick = false;
                             int r = digitalRead(TFT_BL);
                             tft.fillScreen(TFT_BLACK);
                             tft.setTextColor(TFT_GREEN, TFT_BLACK);
                             tft.setTextDatum(MC_DATUM);
                             tft.drawString("Press again to wake up", tft.width() / 2, tft.height() / 2);
                             espDelay(6000);
                             digitalWrite(TFT_BL, !r);

                             tft.writecommand(TFT_DISPOFF);
                             tft.writecommand(TFT_SLPIN);
                             esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
                             esp_deep_sleep_start();
                           });

  btn1.setPressedHandler([](Button2 &b)
                         {
                           Serial.println("Detect Voltage..");
                           btnCick = true;
                         });

  btn2.setPressedHandler([](Button2 &b)
                         {
                           btnCick = false;
                           Serial.println("btn press wifi scan");
                           wifi_scan();
                         });
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  wifiManager.setAPCallback(configModeCallback);
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  // Connect to Wi-Fi
  // Serial.print("Connecting to wifi.");
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hello, world"); });

  // disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();

  // if (H >= 0 && H < 7)
  // {
  //   setColor(0, 0, 50);
  // }
  // //From 07:00 to 12:00
  // else if (H >= 7 && H < 12)
  // {
  //   setColor(50, 50, 100);
  // }
  // else if (H >= 12 && H < 19)
  // {
  //   setColor(180, 180, 180);
  // }
  // else if (H >= 19 && H < 21)
  // {
  //   setColor(50, 50, 100);
  // }
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}