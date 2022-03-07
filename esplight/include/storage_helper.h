#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"
#include "lighttime_struct.h"

#define FORMAT_LITTLEFS_IF_FAILED true

#define FILE_PATH "/data.json"

const size_t capacity = 6144;

class LighTimeStorage {
   public:
    LightTime lightTimes[48];
    int timesSaved;
    String rawJson;

    LighTimeStorage() {
        this->rawJson = "";
    }

    bool setup() {
        if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
            return 0;
        }
        return 1;
    }

    String transformDocumentInStruct(DynamicJsonDocument doc) {
        this->timesSaved = 0;
        this->rawJson = "[";

        int i = 0;
        JsonArray times = doc.as<JsonArray>();

        this->timesSaved = times.size();

        for (JsonObject item : times) {
            serializeJson(item, this->rawJson);

            int h = item["h"];
            int m = item["m"];

            this->lightTimes[i].h = h;
            this->lightTimes[i].m = m;

            JsonArray channels = item["c"];

            for (int ch = 0; ch < channels.size(); ch++) {
                int brightness = channels.getElement(ch);
                // Serial.println(brightness);
                this->lightTimes[i].c[ch] = brightness;
            }
        }

        this->rawJson += "]";

        return this->rawJson;
    }

    void load() {
        DynamicJsonDocument doc(capacity);
        File data = LITTLEFS.open(FILE_PATH, FILE_READ);
        Serial.println("loading json file");
        DeserializationError error;

        if (!data) {
            Serial.println("no json found, creating our");
            // no data, create example json
            char json[] = "[{\"h\":9,\"m\":0,\"c\":[100,100,100,100]},{\"h\": 21,\"m\": 0,\"c\":[0,0,0,0]}]";
            error = deserializeJson(doc, json);
        } else {
            Serial.println("json found :), loading it");
            size_t size = data.size();
            std::unique_ptr<char[]> buf(new char[size]);
            data.readBytes(buf.get(), size);
            Serial.print("Loaded json: ");
            Serial.print(buf.get());
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
        DynamicJsonDocument doc(capacity);
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