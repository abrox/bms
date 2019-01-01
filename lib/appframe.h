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
#include "sandman.h"
#include "batterymonitor.h"

#include <runnable.h>



class AppFrame{
public:
    AppFrame(eQueue_t &eq);

    void run();
    void setUp();
    void init();

private:

    BootType getBootType();
    static const uint16_t _rs=7;
    eQueue_t &_eq;
    runnable_t *_r[_rs];
    AppCtx _appCtx;

};

#endif
