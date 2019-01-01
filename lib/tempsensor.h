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

    void handleMsgIn(const Msg &msg);
    void setUp();
    void init();

private:
    enum class State{
        IDLE,
        WAIT_CONVERSION
    };
    void timerCallBack();

    OneWire oneWire;
    DallasTemperature sensors;
    Ticker _tick;
    State  _state;

    float temp;

};


#endif
