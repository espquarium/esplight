#include <Arduino.h>
#include <Button2.h>
#include <DNSServer.h>
#include <TFT_eSPI.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "esp_adc_cal.h"
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
int vref = 1100;

WiFiManager wifiManager;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

const char* PARAM_MESSAGE = "message";

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

void espDelay(int ms) {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void button_init() {
    btn1.setLongClickHandler(
        [](Button2& b) {
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
            if (!wifiManager.startConfigPortal("ESP32-CONFIG")) {  // Nome da Rede e Senha gerada pela ESP
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

void button_loop() {
    btn1.loop();
    btn2.loop();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Start");
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    if (TFT_BL > 0) {                            // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

    espDelay(5000);

    tft.setRotation(0);
    int i = 5;
    while (i--) {
        tft.fillScreen(TFT_RED);
        espDelay(1000);
        tft.fillScreen(TFT_BLUE);
        espDelay(1000);
        tft.fillScreen(TFT_GREEN);
        espDelay(1000);
    }

    button_init();

    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    } else {
        Serial.println("Default Vref: 1100mV");
    }

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
    if (WiFi.status() != WL_CONNECTED) {  // Se conectado na rede
        wifiManager.autoConnect();
    }
    button_loop();
}
