#include <Arduino.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "time.h"

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

TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

WiFiManager wifiManager;

AsyncWebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

const char* PARAM_MESSAGE = "message";

void button_init() {
    btn1.setLongClickHandler(
        [](Button2& b) {
            btnCick = false;
            int r = digitalRead(TFT_BL);
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("Press again to wake up", tft.width() / 2,
                           tft.height() / 2);
            espDelay(6000);
            digitalWrite(TFT_BL, !r);

            tft.writecommand(TFT_DISPOFF);
            tft.writecommand(TFT_SLPIN);
            esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
            esp_deep_sleep_start();
        });

    // btn1.setPressedHandler(
    //     [](Button2& b) {
    //         Serial.println("Detect Voltage..");
    //         btnCick = true;
    //     });

    btn2.setPressedHandler(
        [](Button2& b) {
            if (!wifiManager.startConfigPortal("ESP32-CONFIG", "12345678")) {  // Nome da Rede e Senha gerada pela ESP
                Serial.println(
                    "Falha ao conectar");  // Se caso não conectar na rede
                // mostra mensagem de falha
                delay(2000);
                ESP.restart();  // Reinicia ESP após não conseguir conexão na
                                // rede
            } else {            // Se caso conectar
                Serial.println("Conectado na Rede!!!");
                ESP.restart();  // Reinicia ESP após conseguir conexão na rede
            }
        });
}

void notFound(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    Serial.begin(115200);

    wifiManager.setAPCallback(configModeCallback);
    // callback para quando se conecta em uma rede, ou seja, quando passa a
    // trabalhar em modo estação
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
    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // server.on("/", HTTP_GET,
    //           [](AsyncWebServerRequest* request) { request->send(200, "text/plain", "Hello, world"); });

    // disconnect WiFi as it's no longer needed
    // WiFi.disconnect(true);
    // WiFi.mode(WIFI_OFF);
}

void loop() {
    if (WiFi.status() != = WL_CONNECTED) {  // Se conectado na rede
        wifiManager.autoConnect();
    }
}

void configModeCallback(WiFiManager* myWiFiManager) {
    Serial.println("Entrou no modo de configuração");
    Serial.println(WiFi.softAPIP());  // imprime o IP do AP
    Serial.println(
        myWiFiManager->getConfigPortalSSID());  // imprime o SSID criado da rede
}

// Callback que indica que salvamos uma nova rede para se conectar (modo
// estação)
void saveConfigCallback() {
    Serial.println("Configuração salva");
}