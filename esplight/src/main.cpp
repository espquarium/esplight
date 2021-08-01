#include <Arduino.h>
#include <Button2.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <storage_helper.h>
#include <tft_helper.h>
#include "esp_adc_cal.h"
#include "time.h"

#define EEPROM_SIZE 1

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 0
#define BUTTON_2 35

Button2 btn1 = Button2(BUTTON_1);
Button2 btn2 = Button2(BUTTON_2);

WiFiManager wifiManager;

int runServer = true;

WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

const char* PARAM_MESSAGE = "message";

// todo https://stackoverflow.com/questions/2548075/c-string-template-library

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

void connectToWifi() {
    String portalName = "ESPLIGHT-WIFI";
    printToTft(portalName);
    printToTft("Acesse seu telefone e conecte para configurar", false, 10);
    if (!wifiManager.autoConnect(portalName.c_str())) {
        printToTft("Falha ao conectar");
        espDelay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        espDelay(5000);
    }

    printToTft("Conectado");
    printToTft("IP: " + WiFi.localIP().toString(), false, 10);
    espDelay(3000);
}

void button_init() {
    btn1.setLongClickHandler(
        [](Button2& b) {
            runServer = false;
            int r = digitalRead(TFT_BL);
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("Pressione novamente para ligar", tft.width() / 2,
                           tft.height() / 2);
            espDelay(6000);
            digitalWrite(TFT_BL, !r);

            tft.writecommand(TFT_DISPOFF);
            tft.writecommand(TFT_SLPIN);
            //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
            // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
            esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
            delay(200);
            esp_deep_sleep_start();
        });

    btn1.setClickHandler(
        [](Button2& b) {
            runServer = true;
            printToTft("Conectado");
            printToTft("IP: " + WiFi.localIP().toString(), false, 10);
        });

    // btn2.setPressedHandler(
    //     [](Button2& b) {
    //         wifiManager.setConfigPortalTimeout(180);
    //         String portalName = "ESPLIGHT-WIFI";
    //         printToTft(portalName);
    //         printToTft("Acesse seu telefone e conecte para configurar", false, 10);
    //         if (!wifiManager.startConfigPortal(portalName.c_str())) {
    //             printToTft("Falha ao conectar");
    //             espDelay(3000);
    //             //reset and try again, or maybe put it to deep sleep
    //             ESP.restart();
    //             espDelay(5000);
    //         }

    //         printToTft("Conectado");
    //         printToTft("IP: " + WiFi.localIP().toString(), false, 10);
    //         espDelay(3000);
    //     });
}

void button_loop() {
    btn1.loop();
    btn2.loop();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Start");

    wifiManager.setConfigPortalTimeout(180);
    startupScreen();

    printToTft("ESPLIGHT");

    button_init();

    connectToWifi();

    server.on("/", HTTP_GET,
              []() { server.send(200, "text/plain", "Hello, world"); });

    server.begin();
}

void loop() {
    button_loop();
    if (runServer) {
        server.handleClient();
    }
}
