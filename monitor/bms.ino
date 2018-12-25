#include <Arduino.h>

#include "Ticker.h"
#include "FS.h"

#include "commondefs.h"
#include "eventqueue.h"
#include "appframe.h"

eQueue_t eQueue;

AppFrame app(eQueue);


void setup(void) {
  
    Serial.begin(115200);
    Serial.println("Battery monitor starting...");
    
    if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
    
    app.setUp(); 
    app.init();   
}

void loop(void) {
    app.run();
}
