#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H
/*
MIT License

Copyright (c) 2019 Jukka-Pekka Sarjanen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <Arduino.h>
#include <Ticker.h>

#include "commondefs.h"

#include "ina219.h"


///Current Sensor.
///Responsible current meassuring and
///updating result to _appCtx where Batterymonitor can read them.
///
class CurrentSensor: public runnable_t{
public:
    CurrentSensor(eQueue_t &eq,const INA219::t_i2caddr &addr);

    void handleMsgIn(const Msg &msg);

    ///Sensor setup.
    ///Reading sensor configuration
    ///and initialize sensor.
    ///
    void setUp();

    ///Initialize meassuring.
    ///Request first meassurement
    /// and setup timer for futer ones.
    ///
    void init();

private:
    void timerCallBack();

    INA219 _sensor;
    Ticker _tick;

};
#endif
