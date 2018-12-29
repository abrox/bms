#ifndef SANDMAN_H
#define SANDMAN_H

#include <Arduino.h>

#include "commondefs.h"




class SandMan: public runnable_t{
  public:
  SandMan(eQueue_t &eq);

  void handleMsgIn(const event_t &msg);

  bool setUp();
  void executeAlways();
  private:
  const uint32_t NO_FLAGS     = 0;
  const uint32_t DISPLAY_ON   = 1;
  const uint32_t MQTT_ON      = 2;
  const uint32_t WAIT_CURRENT = 4;
  const uint32_t WAIT_TEMP    = 8;
  uint32_t _flags;

  void setFlags(const uint32_t &flags){
    _flags|= flags;
  }

  void clearFlags(const uint32_t &flags){
      _flags ^= flags;
  }

  bool timeToSleep(){
      return _flags == NO_FLAGS;
  }

};


#endif
