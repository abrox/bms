#include "appframe.h"
#include "FS.h"


Buttons       b(eQueue,BUTTON_PIN);
TempSensor    t(eQueue,ONE_WIRE_BUS);
CurrentSensor c(eQueue,INA219::I2C_ADDR_41);
MqttClient    m(eQueue);
Display       d(eQueue);
SandMan       s(eQueue);


AppFrame::AppFrame(eQueue_t &eq): _eq(eq),_r{ &b,&t,&c,&m,&d,&s},_appCtx(eq)
{

  ;
}

bType_t AppFrame::AppFrame::getBootType()
{
  bType_t type;
  ConfigFileProvider& c=ConfigFileProvider::getInstance();

  if( c.getRTCMemStatus() ){
      type = WARM_BOOT;
  }else
   if( c.configFilesExist() ){
       c.setRTCMemStatus();
       type = COLD_BOOT;
  }else{
       type = FIRST_BOOT;
  }
  
  return type;
}

bool AppFrame::setUp()
{
    _appCtx._bootType = getBootType();
  
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
