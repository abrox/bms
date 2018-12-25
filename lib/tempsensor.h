#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <Arduino.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "commondefs.h"

class TempSensor: public runnable_t{
  public:
  TempSensor(eQueue_t &eq,const uint8_t bus);

  void handleMsgIn(const event_t &msg);
  bool setUp();
  void init();
 
  private:
  enum state{
    IDLE,
    WAIT_CONVERSION
  };
  void timerCallBack();
  
  OneWire oneWire;
  DallasTemperature sensors;
  Ticker _tick;
  state  _state;

  float temp;
  
};


#endif
