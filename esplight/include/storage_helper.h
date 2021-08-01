#include <ArduinoJson.h>
#include <nvs.h>
#include <string>

// todo https://forum.arduino.cc/t/save-array-of-struct-in-eeprom/654104/2
struct LighTime {
    int hours;
    int minutes;
    int ch[4];  // 4 channels
};

class NVS {
    NVS() {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);
    }
}