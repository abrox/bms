#include <Arduino.h>
#include <Wire.h>

#include "Ticker.h"
#include "FS.h"




#include "commondefs.h"
#include "eventqueue.h"
#include "appframe.h"

eQueue_t eQueue;

AppFrame app(eQueue);

DS1307 rtc;
void setup(void) {

    Serial.begin(115200);
    Serial.println("Battery monitor starting...");
    
    if( !SPIFFS.begin() ) {
        Serial.println("Failed to mount file system");
        return;
    }
    
    Wire.begin();
    rtc.begin();

    if( !rtc.isrunning() ) {
        Serial.println("RTC is NOT running!");
       // following line sets the RTC to the date & time this sketch was compiled
       rtc.adjust(DateTime(__DATE__, __TIME__));
    }
     
    app.setUp(); 
    app.init();   
}

void loop(void) {
    app.run();
}
