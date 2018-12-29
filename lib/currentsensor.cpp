#include "currentsensor.h"


CurrentSensor::CurrentSensor(eQueue_t &eq,const INA219::t_i2caddr &addr):Runnable(eq),monitor(addr)
{

}

void CurrentSensor::handleMsgIn(const event_t &msg)
{
   if( msg == CURRENT_TIMEOUT ||
       msg == CURRENT_REQ ){
      Serial.print("raw shunt voltage: ");
  Serial.println(monitor.shuntVoltageRaw());
  
  Serial.print("raw bus voltage:   ");
  Serial.println(monitor.busVoltageRaw());

  Serial.println("--");

  Serial.print("shunt voltage: ");
  Serial.print(monitor.shuntVoltage() * 1000, 4);
  Serial.println(" mV");

  Serial.print("shunt current: ");
  Serial.print(monitor.shuntCurrent() * 1000, 4);
  Serial.println(" mA");

  Serial.print("bus voltage:   ");
  Serial.print(monitor.busVoltage(), 4);
  Serial.println(" V");

  Serial.print("bus power:     ");
  Serial.print(monitor.busPower() * 1000, 4);
  Serial.println(" mW");

  Serial.println(" ");
  Serial.println(" ");
  _eq.putQ(SOC_UPDATE);
   }
  
}

bool CurrentSensor::setUp()
{
    monitor.begin();
    monitor.configure();
    monitor.calibrate();
    return true;
}
void CurrentSensor::init()
{
    _eq.putQ(CURRENT_REQ);
}
