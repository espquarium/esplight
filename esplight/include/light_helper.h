#include "analogWrite.h"
#include "lighttime_struct.h"

#define MAX_BRIGHTNESS 100

int PINS[MAX_CHANNELS] = {36, 39, 41, 40};

class LightHelper {
   public:
    bool forceLight;

    LightHelper() {
        this->forceLight = true;
    }

    void
    allChannels(int brightness) {
        for (int ch : PINS) {
            analogWrite(ch, brightness, MAX_BRIGHTNESS);
        }
    }

    void
    perChannel(int channels[MAX_CHANNELS]) {
        for (int ch = 0; ch < MAX_CHANNELS; ch++) {
            analogWrite(PINS[ch], ch, MAX_BRIGHTNESS);
        }
    }

    void loop(Date timeNow, int timesSaved, LightTime lightTimes[48]) {
        if (this->forceLight) {
            allChannels(MAX_BRIGHTNESS);
        } else {
            for (int i = timesSaved; i != 0; i--) {
                LightTime lightSaved = lightTimes[i];
                if (timeNow.h >= lightSaved.h && timeNow.m >= lightSaved.m) {
                    return this->perChannel(lightSaved.c);
                }
            }
        }
    }

    void setForceLight(bool force) {
        this->forceLight = force;
    }
};