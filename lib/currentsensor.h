#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include <Arduino.h>

#include "commondefs.h"

#include "ina219.h"



class CurrentSensor: public runnable_t{
  public:
  CurrentSensor(eQueue_t &eq,const INA219::t_i2caddr &addr);

  void handleMsgIn(const event_t &msg);
  bool setUp();

  private:
  INA219 monitor;

  
};


#endif
