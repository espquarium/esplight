#include "NTPClient.h"
#include "WiFi.h"

WiFiUDP udp;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;

NTPClient timeClient = NTPClient(udp, ntpServer, gmtOffset_sec, 60000);

struct Date {
    int hours;
    int minutes;
};

class NTPHelper {
   public:
    NTPHelper() {
    }

    Date getTime() {
        timeClient.update();

        Date date;

        date.hours = timeClient.getHours();
        date.minutes = timeClient.getMinutes();

        return date;
    }
};