#include "NTPClient.h"
#include "WiFi.h"

WiFiUDP udp;

const char* ntpServer = "201.49.148.135";
const long gmtOffset_sec = -10800;

NTPClient timeClient = NTPClient(udp, ntpServer, gmtOffset_sec, 60000);

class NTPHelper {
   public:
    NTPHelper() {
    }

    Date getTime() {
        while (!timeClient.update()) {
            timeClient.forceUpdate();
        }

        Date date;

        date.h = timeClient.getHours();
        date.m = timeClient.getMinutes();

        return date;
    }
};