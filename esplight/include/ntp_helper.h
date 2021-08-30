
#include "NTPClient.h"

struct Date {
    int hours;
    int minutes;
};

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;

class LightHelper {
   public:
    NTPClient client;

    LightHelper() {
        this->client = ntpClient(
            udp,            //socket udp
            ntpServer,      //URL do server NTP
            gmtOffset_sec,  //Deslocamento do horário em relacão ao GMT 0
            60000);         //Intervalo entre verificações online;
    }

    Date getDate() {
        Date date;

        date.hours = ntpClient.getHours();
        date.minutes = ntpClient.getMinutes();
        return date;
    }

    void
    perChannel(int channels[MAX_CHANNELS]) {
        for (int ch = 0; ch < MAX_CHANNELS; ch++) {
            analogWrite(PINS[ch], ch, MAX_BRIGHTNESS);
        }
    }

    void loop() {
        if (this->forceLight) {
            allChannels(MAX_BRIGHTNESS);
        }
    }

    void setForceLight(bool force) {
        this->forceLight = force;
    }
};