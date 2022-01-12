#include "NTPClient.h"
#include "WiFi.h"

WiFiUDP udp;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;

NTPClient timeClient = NTPClient(udp, ntpServer, gmtOffset_sec, 60000);



class NTPHelper {
   public:
    NTPHelper() {
    }

    Date getTime() {
        timeClient.update();

        Date date;

        date.h = timeClient.getHours();
        date.m = timeClient.getMinutes();

        return date;
    }
};