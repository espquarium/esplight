#include "analogWrite.h"

#define MAX_BRIGHTNESS 100
#define CH1 27
#define CH2 26
#define CH3 25
#define CH4 33

class LightHelper {
   public:
    bool forceLight;

    LightHelper() {
        forceLight = true;
    }

    void
    allChannels(int brightness) {
        analogWrite(CH1, brightness, MAX_BRIGHTNESS);
        analogWrite(CH2, brightness, MAX_BRIGHTNESS);
        analogWrite(CH3, brightness, MAX_BRIGHTNESS);
        analogWrite(CH4, brightness, MAX_BRIGHTNESS);
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