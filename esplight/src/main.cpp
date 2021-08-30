// todo https://stackoverflow.com/questions/2548075/c-string-template-library
#include "Arduino.h"
#include "Button2.h"
#include "DNSServer.h"
#include "NTPClient.h"
#include "WebServer.h"
#include "WiFi.h"
#include "WiFiManager.h"
#include "WiFiUdp.h"
#include "esp_adc_cal.h"
#include "light_helper.h"
#include "storage_helper.h"
#include "tft_helper.h"
#include "time.h"

#define EEPROM_SIZE 1

#define BUTTON_1 0
#define BUTTON_2 35

Button2 btn1 = Button2(BUTTON_1);
Button2 btn2 = Button2(BUTTON_2);

WiFiUDP udp;
WiFiManager wifiManager;

int runServer = true;

LighTimeStorage lightStorage = LighTimeStorage();
WebServer server(80);
LightHelper light = LightHelper();

const char* PARAM_MESSAGE = "message";

void setupNTP() {
    //Inicializa o client NTP
    ntpClient.begin();

    //Espera pelo primeiro update online
    Serial.println("Waiting for first update");
    while (!ntpClient.update()) {
        Serial.print(".");
        ntpClient.forceUpdate();
        delay(500);
    }

    Serial.println();
    Serial.println("First Update Complete");
}

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
}

void button_loop() {
    btn1.loop();
    btn2.loop();
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
    // printToTft(resText, false, 40);
    String resText = "{\"force\":";
    resText += (light.forceLight ? "true" : "false");
    resText += "}";
    setCrossOrigin();
    server.send(200, "application/json", resText);
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
    Serial.begin(9600);
    Serial.println("Start");
    lightStorage.setup();

    wifiManager.setConfigPortalTimeout(180);

    setupNTP();

    startupScreen();

    printToTft("ESPLIGHT");

    button_init();

    lightStorage.load();

    connectToWifi();

    server.on("/", HTTP_GET,
              []() { server.send(200, "text/plain", "Welcome to reeflight :)"); });

    server.on("/toggle", HTTP_GET,
              handleToggle);

    server.on("/verify", HTTP_GET,
              handleVerify);

    server.on("/light-times", HTTP_GET,
              handleLightTimes);

    server.on("/light-times", HTTP_OPTIONS, sendCrossOriginHeader);

    server.on("/light-times", HTTP_PUT,
              handleUpdateLightTimes);

    server.onNotFound(handleNotFound);

    server.begin();
}

void loop() {
    Date date = getDate();
    button_loop();
    light.loop();
    if (runServer) {
        server.handleClient();
    }
}
