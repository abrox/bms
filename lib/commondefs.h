#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include <runnable.h>
#include "RTClib.h"

#include "msg.h"

//#define DEBUG
#ifdef DEBUG  
  #define DPRINT(...)    Serial.print(__VA_ARGS__) 
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__) 
#else
  #define DPRINT(...) 
  #define DPRINTLN(...)
#endif


#ifdef DEBUG
  #define SET_NAME(s) _stName = PSTR(#s)
#else
   #define SET_NAME(s)
#endif

#define CHANGE_STATE(s) \
       SET_NAME(s);\
       DPRINTLN(_stName);\
       _st     = s;


enum class BootType{
    UNDEFINED,
    FIRST_BOOT,
    COLD_BOOT,
    WARM_BOOT
};

typedef EventQueue<Msg> eQueue_t;

class AppCtx{

public:
    AppCtx(eQueue_t &eq):_eq(eq){;}

    void updateStatusDisplay( const char * statusMsg){
        _statusMsg = statusMsg;
         DPRINTLN(_statusMsg);
        _eq.putQ(Msg::UPDATE_STATUS_DISLAY);
    }

    const char * _statusMsg{nullptr};
    CurrentData  _currentData;
    BatteryStats _batteryStats;
    BootType     _bootType{BootType::UNDEFINED};
    bool         _sleepNow{false};
    uint32_t     _startTime{0};///<time since last BootType::COLD_BOOT
    eQueue_t &_eq;
};



typedef Runnable<Msg,AppCtx> runnable_t;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 0

extern eQueue_t eQueue;
extern DS1307 rtc;

//SDA 4
//Scl 5

//reset -> GPI016

#define OLED_MOSI  13 //D1
#define OLED_CLK   14 //D0
#define OLED_DC     2 
#define OLED_CS    U8X8_PIN_NONE /*15*/
#define OLED_RESET 15/*U8X8_PIN_NONE /*12*/


#define BUTTON_PIN 12


#endif
