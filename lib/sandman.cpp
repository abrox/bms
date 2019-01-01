#include "sandman.h"


SandMan::SandMan(eQueue_t &eq):Runnable(eq),_flags(NO_FLAGS)
{

}

void SandMan::handleMsgIn(const Msg &msg)
{
    if( msg == Msg::BUTTON_DOWN ){
       setFlags(DISPLAY_ON);
    }else
     if( msg == Msg::DISPLAY_OFF_TIMEOUT ){
         clearFlags(DISPLAY_ON);
    }else
     if( msg == Msg::SOC_UPDATE ){
         clearFlags(WAIT_CURRENT);
    }else
     if(msg == Msg::START_MQTT){
         setFlags(MQTT_ON);
    }else
     if(msg == Msg::MQTT_FAILED){
        clearFlags(MQTT_ON);
    }

    if( timeToSleep() ){
        _appCtx->_sleepNow=true;
    }

}

void SandMan::setUp()
{
    setFlags(WAIT_CURRENT);
}


