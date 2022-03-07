#include "analogWrite.h"
#include "lighttime_struct.h"

int MAX_BRIGHTNESS = 100;

int PINS[MAX_CHANNELS] = {2, 22, 1, 3};

class LightHelper {
   public:
    bool forceLight;

    LightHelper() {
        this->forceLight = false;
    }

    void setup() {
        this->allChannels(0);
    }

    void
    allChannels(int brightness) {
        for (int ch : PINS) {
            analogWrite(ch, brightness, MAX_BRIGHTNESS);
        }
    }

    void
    perChannel(int channels[MAX_CHANNELS]) {
        for (int channelNumber = 0; channelNumber < MAX_CHANNELS; channelNumber++) {
            analogWrite(PINS[channelNumber], channels[channelNumber], MAX_BRIGHTNESS);
        }
    }

    void loop(Date timeNow, int timesSaved, LightTime lightTimes[48]) {
        if (this->forceLight) {
            allChannels(MAX_BRIGHTNESS);
        } else {
            for (size_t i = 0; i < timesSaved; i++) {
                LightTime lightSaved = lightTimes[i];
                // Serial.println("While ");
                // Serial.print(lightSaved.m);
                if (lightSaved.h <= timeNow.h && lightSaved.m <= timeNow.m) {
                    this->perChannel(lightSaved.c);
                    return;
                }
            }
        }
    }

    void setForceLight(bool force) {
        this->forceLight = force;
    }
};