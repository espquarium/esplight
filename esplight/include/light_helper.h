#include "analogWrite.h"
#include "lighttime_struct.h"

#define MAX_BRIGHTNESS 100

int PINS[MAX_CHANNELS] = {23, 22, 1, 3};
// int PINS[MAX_CHANNELS] = {2, 22, 1, 3};

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
        } else if (timesSaved > 0) {
            int i = timesSaved - 1;
            while (i--) {
                LightTime lightSaved = lightTimes[i];
                // Serial.println("While ");
                // Serial.println(lightSaved.h);
                if (lightSaved.h < timeNow.h || (lightSaved.h == timeNow.h && lightSaved.m <= timeNow.m)) {
                    return this->perChannel(lightSaved.c);
                }
            };
        }
    }

    void setForceLight(bool force) {
        this->forceLight = force;
    }
};