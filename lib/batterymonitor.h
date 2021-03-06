#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <Arduino.h>

#include "commondefs.h"
#include "battery.h"

class BatteryMonitor: public runnable_t{
public:
    BatteryMonitor(eQueue_t &eq);

    void handleMsgIn(const Msg &msg);

    void setUp();
    void tearDown();
    void executeAlways();
private:
    bms::Battery _bat;
    BatteryCfg _cfg;
    BatteryCtx _ctx;
};
#endif
