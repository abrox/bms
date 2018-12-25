#include "appframe.h"
#include "FS.h"


Buttons       b(eQueue,BUTTON_PIN);
TempSensor    t(eQueue,ONE_WIRE_BUS);
CurrentSensor c(eQueue,INA219::I2C_ADDR_41);
MqttClient    m(eQueue);
Display       d(eQueue);


AppFrame::AppFrame(eQueue_t &eq): _eq(eq),_r{ &b,&t,&c,&m,&d},_appCtx(eq)
{

  ;
}
bool AppFrame::ReadRTCMem()
{
 
 return false;
}

AppFrame::bType_t AppFrame::getBootTypeAndRTCMem()
{
  bType_t rc;
  if( ReadRTCMem() ){
    rc = WARM_BOOT;
  }else
   if(SPIFFS.exists("/config.json")){
    rc = COLD_BOOT;
  }else{
    rc = FIRST_BOOT;
  }
  
  return rc;
}
void AppFrame::readConfigFromSerial()
{
  
}
bool AppFrame::setUp()
{
  switch(getBootTypeAndRTCMem()){
    case FIRST_BOOT:
        readConfigFromSerial();
    break;
    
  }
  
  for(int i=0;i < _rs;i++)
      _r[i]->setUp(&_appCtx);
  return true;
}
bool AppFrame::init()
{
  for(int i=0;i < _rs;i++)
      _r[i]->init();
      
  return true;
}

void AppFrame::run()
{
  event_t event;
  while(_eq.getQ(event)){
      for(int i=0;i < _rs;i++){
          _r[i]->handleMsgIn(event);
      }
  }
  
  for(int i=0;i < _rs;i++){
          _r[i]->executeAlways();
      }
}
