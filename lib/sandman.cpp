#include "sandman.h"


SandMan::SandMan(eQueue_t &eq):Runnable(eq),_flags(NO_FLAGS)
{

}
void SandMan::executeAlways()
{
   if( timeToSleep() ){
       //Sleep for 1 sec
       ESP.deepSleep(1e6);
   }
}

void SandMan::handleMsgIn(const event_t &msg)
{
    if( msg == BUTTON_DOWN ){
       setFlags(DISPLAY_ON);
    }else
     if( msg == DISPLAY_OFF_TIMEOUT ){
         clearFlags(DISPLAY_ON);
    }else
     if( msg == SOC_UPDATE ){
         clearFlags(WAIT_CURRENT);
    }else
     if(msg == START_MQTT){
         setFlags(MQTT_ON);
    }else
     if(msg == MQTT_FAILED){
        clearFlags(MQTT_ON);
     }

}

bool SandMan::setUp()
{
    setFlags(WAIT_CURRENT);
    return true;
}

