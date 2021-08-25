#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"

#define FORMAT_LITTLEFS_IF_FAILED true

#define FILE_PATH "/data.json"

struct LighTime {
    int h;
    int m;
    int c[4];  // 4 channels
};

class LighTimeStorage {
   public:
    LighTime lightTimes[48];
    int timesSaved;
    String rawJson;

    LighTimeStorage() {
    }

    int setup() {
        if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
            return 0;
        }
    }

    String transformDocumentInStruct(DynamicJsonDocument doc) {
        int i = 0;
        for (JsonObject item : doc.as<JsonArray>()) {
            int h = item["h"];
            int m = item["m"];

            JsonArray c = item["c"];
            int c_0 = c[0];
            int c_1 = c[1];
            int c_2 = c[2];
            int c_3 = c[3];

            this->lightTimes[i].h = h;
            this->lightTimes[i].m = m;

            this->lightTimes[i].c[0] = c_0;
            this->lightTimes[i].c[1] = c_1;
            this->lightTimes[i].c[2] = c_2;
            this->lightTimes[i].c[3] = c_3;

            i++;
        }
        this->timesSaved = i;

        this->rawJson = "";

        serializeJson(doc, this->rawJson);

        return this->rawJson;
    }

    void load() {
        DynamicJsonDocument doc(6144);
        File data = LITTLEFS.open(FILE_PATH, "r");
        Serial.println("loading json file");
        DeserializationError error;

        if (!data) {
            Serial.println("no json found, creating our");
            // no data, create example json
            char json[] = "[{ \"h\": 10, \"m\": 0, \"c\": [100, 100, 100, 100]},{ \"h\": 18, \"m\": 0, \"c\": [100, 100, 100, 100]}]";
            error = deserializeJson(doc, json);
        } else {
            Serial.println("json found :), loading it");
            size_t size = data.size();
            std::unique_ptr<char[]> buf(new char[size]);
            data.readBytes(buf.get(), size);

            error = deserializeJson(doc, buf.get());
            data.close();
        }

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        Serial.println("loaded json from disc");

        transformDocumentInStruct(doc);
    }

    void save(const String json) {
        DynamicJsonDocument doc(6144);
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        Serial.printf("Writing file: %s\r\n", FILE_PATH);

        File file = LITTLEFS.open(FILE_PATH, FILE_WRITE);
        if (!file) {
            Serial.println("- failed to open file for writing");
            return;
        }
        if (file.print(json)) {
            Serial.println("- file written");
        } else {
            Serial.println("- write failed");
        }
        file.close();

        transformDocumentInStruct(doc);
    }

    String getTimesAsJson() {
        return this->rawJson;
    }
};