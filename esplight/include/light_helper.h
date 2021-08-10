#include "analogWrite.h"

#define MAX_BRIGHTNESS 100
#define MAX_CHANNELS 4

int PINS[MAX_CHANNELS] = {27, 6, 25, 33};

class LightHelper {
   public:
    bool forceLight;

    LightHelper() {
        forceLight = true;
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

    void loop() {
        if (forceLight) {
            allChannels(MAX_BRIGHTNESS);
        }
    }

    void setForceLight(bool force) {
        forceLight = force;
    }
};