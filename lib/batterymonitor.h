#define BATTERYMONITOR_H

#include <Arduino.h>

#include "commondefs.h"


class BatteryMonitor: public runnable_t{
  public:
  BatteryMonitor(eQueue_t &eq);

  void handleMsgIn(const event_t &msg);

  void setUp();
  void tearDown();
  void executeAlways();
  private:

};
