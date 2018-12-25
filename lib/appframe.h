#ifndef APPFRAME_H
#define APPFRAME_H

#include <Arduino.h>

#include "commondefs.h"

#include "ina219.h"
#include "tempsensor.h"
#include "currentsensor.h"
#include "buttons.h"
#include "display.h"
#include "mqttclient.h"


#include <runnable.h>



class AppFrame{
  public:
  AppFrame(eQueue_t &eq);
           
  void run();
  bool setUp();
  bool init();

  private:
  typedef enum bType_t{
    FIRST_BOOT,
    COLD_BOOT,
    WARM_BOOT
    
  }bType_t;
  
  bType_t getBootTypeAndRTCMem();
  bool ReadRTCMem();
  void readConfigFromSerial();
  static const uint16_t _rs=5;
  eQueue_t &_eq;
  runnable_t *_r[_rs];
  AppCtx _appCtx;
  
};

#endif
