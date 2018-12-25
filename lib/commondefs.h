#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include <runnable.h>


#define DEBUG  
#ifdef DEBUG  
  #define DPRINT(...)    Serial.print(__VA_ARGS__) 
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__) 
#else
  #define DPRINT(...) 
  #define DPRINTLN(...)
#endif

typedef enum event_t{
  MENU_TIMEOUT,
  UPDATE_STATUS_DISLAY,
  BUTTON_DOWN,
  BUTTON_DOWN_TWO_SEC,
  BUTTON_DOWN_LONG,
  BUTTON_DOWN_EXTLONG,
  BUTTON_UP,
  IDLE_TIMEOUT,
  MEASSURE_TIMEOUT,
  CURRENT_TIMEOUT,
  START_MQTT,
  MQTT_CLIENT_TIMEOUT,
  MQTT_RUNNING,
  MQTT_FAILED
}event_t;

typedef EventQueue<event_t> eQueue_t;

class AppCtx{
    public:
    AppCtx(eQueue_t &eq):_eq(eq){;}
    
    void updateStatusDisplay( const char * statusMsg){
        _statusMsg = statusMsg;
         DPRINTLN(_statusMsg);
        _eq.putQ(UPDATE_STATUS_DISLAY);
  }
  
  const char * _statusMsg{NULL};
  private:
  eQueue_t &_eq;
};



typedef Runnable<event_t,AppCtx> runnable_t;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 0

extern eQueue_t eQueue;

//SDA 4
//Scl 5

//reset -> GPI016

#define OLED_MOSI  13 //D1
#define OLED_CLK   14 //D0
#define OLED_DC     2 
#define OLED_CS    U8X8_PIN_NONE /*15*/
#define OLED_RESET 15/*U8X8_PIN_NONE /*12*/


#define BUTTON_PIN 12

#define NET_CONFIG_FILE PSTR("/config.json")

#endif
