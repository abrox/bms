#include "tempsensor.h"



void TempSensor::timerCallBack(){
    _eq.putQ(Msg::MEASSURE_TIMEOUT);
}

TempSensor::TempSensor(eQueue_t &eq,const uint8_t bus):Runnable(eq),oneWire(bus), sensors(&oneWire),temp(0.0),_state(State::IDLE)
{

}

void TempSensor::handleMsgIn(const Msg &msg)
{

    switch(_state){
    case State::IDLE:
        if(msg == Msg::MEASSURE_TIMEOUT){
            sensors.setWaitForConversion(false);  // makes it async
            sensors.requestTemperatures();
            sensors.setWaitForConversion(true);
            _tick.once(0.4, std::bind(&TempSensor::timerCallBack, this));
            _state = State::WAIT_CONVERSION;
        }
        break;
    case State::WAIT_CONVERSION:
        if(msg == Msg::MEASSURE_TIMEOUT){
            temp = sensors.getTempCByIndex(0);
            Serial.print("Temp:");
            Serial.println(temp);
            _tick.once(10, std::bind(&TempSensor::timerCallBack, this));
            _state = State::IDLE;
        }
        break;
    }

}

void TempSensor::setUp()
{
    // Start up the library
    sensors.begin();
}
void TempSensor::init()
{
    //_eq.putQ(MEASSURE_TIMEOUT);
}

