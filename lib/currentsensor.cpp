#include "currentsensor.h"

//Debug printout
void printCurrent(INA219& monitor){
    DPRINT("raw shunt voltage: ");
    DPRINTLN(monitor.shuntVoltageRaw());

    DPRINT("raw bus voltage:   ");
    DPRINTLN(monitor.busVoltageRaw());

    DPRINTLN("--");

    DPRINT("shunt voltage: ");
    DPRINT(monitor.shuntVoltage() * 1000, 4);
    DPRINTLN(" mV");

    DPRINT("shunt current: ");
    DPRINT(monitor.shuntCurrent() * 1000, 4);
    DPRINTLN(" mA");

    DPRINT("bus voltage:   ");
    DPRINT(monitor.busVoltage(), 4);
    DPRINTLN(" V");

    DPRINT("bus power:     ");
    DPRINT(monitor.busPower() * 1000, 4);
    DPRINTLN(" mW");

    DPRINTLN(" ");
    DPRINTLN(" ");

}

CurrentSensor::CurrentSensor(eQueue_t &eq,const INA219::t_i2caddr &addr):Runnable(eq),monitor(addr)
{

}

void CurrentSensor::handleMsgIn(const Msg &msg)
{
    if( msg == Msg::CURRENT_TIMEOUT ||
        msg == Msg::CURRENT_REQ ){

        _appCtx->_currentData.volt = monitor.busVoltage();
        _appCtx->_currentData.cu   = monitor.shuntCurrent();

        printCurrent(monitor);
        _eq.putQ(Msg::SOC_UPDATE);
   }
  
}

void CurrentSensor::setUp()
{
    monitor.begin();
    monitor.configure();
    monitor.calibrate();
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
