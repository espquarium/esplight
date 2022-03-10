#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"
#include "lighttime_struct.h"

#define FORMAT_LITTLEFS_IF_FAILED true

#define FILE_PATH "/data.json"

#define JSON_CAPACITY 12000

#define MAX_LIGHT_TIMES 48

class LighTimeStorage {
   public:
    LightTime lightTimes[MAX_LIGHT_TIMES];
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

    void transformDocumentInStruct(DynamicJsonDocument& doc) {
        this->timesSaved = 0;
        this->rawJson = "[";

        int i = 0;
        JsonArray times = doc.as<JsonArray>();
        for (JsonObject item : times) {
            serializeJson(item, this->rawJson);

            int h = item["h"];
            int m = item["m"];

            this->lightTimes[i].h = h;
            this->lightTimes[i].m = m;

            // this->lightTimes[i] = {
            //     h,
            //     m,
            //     item["c"].as<JsonArray>(),
            // };

            // Serial.println(F("==================="));
            // Serial.println(F("Exibindo tempo "));
            // Serial.print(this->lightTimes[i].h);
            // Serial.print(":");
            // Serial.print(this->lightTimes[i].m);

            // Serial.println(F("==================="));
            // Serial.println(F("Exibindo brightness "));
            JsonArray channels = item["c"];
            for (int ch = 0; ch < channels.size(); ch++) {
                int brightness = channels.getElement(ch);
                Serial.print(brightness);
                Serial.print(",");
                this->lightTimes[i].c[ch] = brightness;
            }

            i++;
        }
        this->timesSaved = i + 1;
        this->rawJson += "]";
    }

    void load() {
        DynamicJsonDocument doc(JSON_CAPACITY);
        File data = LITTLEFS.open(FILE_PATH, FILE_READ);
        Serial.println("loading json file");
        DeserializationError error;

        if (!data) {
            Serial.println(F("no json found, creating our"));
            // no data, create example json
            error = deserializeJson(doc, F("[{\"h\":9,\"m\":0,\"c\":[100,100,100,100]},{\"h\": 21,\"m\": 0,\"c\":[0,0,0,0]}]"));
        } else {
            Serial.println(F("json found :), loading it"));
            // size_t size = data.size();
            // std::unique_ptr<char[]> buf(new char[size]);
            // data.readBytes(buf.get(), size);
            // Serial.print(F("Loaded json: "));
            // Serial.print(buf.get());
            error = deserializeJson(doc, data);
            data.close();
        }

        if (error) {
            Serial.println(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        Serial.println(F("loaded json from disc"));

        transformDocumentInStruct(doc);
    }

    void save(const String json) {
        DynamicJsonDocument doc(JSON_CAPACITY);
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        Serial.printf("Writing file: %s\r\n", FILE_PATH);

        File file = LITTLEFS.open(FILE_PATH, FILE_WRITE);
        if (!file) {
            Serial.println(F("- failed to open file for writing"));
            return;
        }
        if (file.print(json)) {
            Serial.println(F("- file written"));
        } else {
            Serial.println(F("- write failed"));
        }
        file.close();

        transformDocumentInStruct(doc);
    }

    String getTimesAsJson() {
        return this->rawJson;
    }
};