#include "analogWrite.h"

#define MAX_BRIGHTNESS 100
#define MAX_CHANNELS 4

int PINS[MAX_CHANNELS] = {27, 26, 25, 33};

class LightHelper
{
public:
    bool forceLight;

    LightHelper()
    {
        this->forceLight = true;
    }

    void
    allChannels(int brightness)
    {
        for (int ch : PINS)
        {
            analogWrite(ch, brightness, MAX_BRIGHTNESS);
        }
    }

    void
    perChannel(int channels[MAX_CHANNELS])
    {
        for (int ch = 0; ch < MAX_CHANNELS; ch++)
        {
            analogWrite(PINS[ch], ch, MAX_BRIGHTNESS);
        }
    }

    void loop(Date timeNow, int timesSaved, LighTime lightTimes[48])
    {
        if (this->forceLight)
        {
            allChannels(MAX_BRIGHTNESS);
        }
        else
        {
            for (int i = timesSaved; i != 0; i--)
            {
                LightTime lightSaved = lightTimes[i];
                if (timeNow.h >= lightSaved.h && timeNow.m >= lightSaved.m)
                {
                    return this->perChannel(lightSaved.c);
                }
            }
        }
    }

    void setForceLight(bool force)
    {
        this->forceLight = force;
    }
};