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
#include "currentsensor.h"

//Debug printout
void printCurrent(INA219& sensor){
    DPRINT("raw shunt voltage: ");
    DPRINTLN(sensor.shuntVoltageRaw());

    DPRINT("raw bus voltage:   ");
    DPRINTLN(sensor.busVoltageRaw());

    DPRINTLN("--");

    DPRINT("shunt voltage: ");
    DPRINT(sensor.shuntVoltage() * 1000, 4);
    DPRINTLN(" mV");

    DPRINT("shunt current: ");
    DPRINT(sensor.shuntCurrent() * 1000, 4);
    DPRINTLN(" mA");

    DPRINT("bus voltage:   ");
    DPRINT(sensor.busVoltage(), 4);
    DPRINTLN(" V");

    DPRINT("bus power:     ");
    DPRINT(sensor.busPower() * 1000, 4);
    DPRINTLN(" mW");

    DPRINTLN(" ");
    DPRINTLN(" ");

}

CurrentSensor::CurrentSensor(eQueue_t &eq,const INA219::t_i2caddr &addr):Runnable(eq),_sensor(addr)
{

}

void CurrentSensor::handleMsgIn(const Msg &msg)
{
    if( msg == Msg::CURRENT_REQ ){

        _appCtx->_currentData.volt = _sensor.busVoltage();
        _appCtx->_currentData.cu   = _sensor.shuntCurrent();

        printCurrent(_sensor);
        _eq.putQ(Msg::SOC_UPDATE);
   }
  
}

void CurrentSensor::setUp()
{
    _sensor.begin();
    _sensor.configure();
    _sensor.calibrate();
}
void CurrentSensor::init()
{
    _eq.putQ(Msg::CURRENT_REQ);
    _tick.attach(1, std::bind(&CurrentSensor::timerCallBack, this));
}
void CurrentSensor::timerCallBack()
{
   _eq.putQ(Msg::CURRENT_REQ);
}
