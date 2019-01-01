#include "appframe.h"
#include "FS.h"

#include "configmanager.h"

Buttons        b(eQueue,BUTTON_PIN);
TempSensor     t(eQueue,ONE_WIRE_BUS);
CurrentSensor  c(eQueue,INA219::I2C_ADDR_41);
BatteryMonitor bm(eQueue);
MqttClient     m(eQueue);
Display        d(eQueue);
SandMan        s(eQueue);


AppFrame::AppFrame(eQueue_t &eq): _eq(eq),_r{ &b,&t,&c,&bm,&m,&d,&s},_appCtx(eq)
{

    ;
}

BootType AppFrame::AppFrame::getBootType()
{
    BootType type;
    ConfigManager& c=ConfigManager::getInstance();

    if( c.getRTCMemStatus() ){
        type = BootType::WARM_BOOT;
    }else
     if( c.configFilesExist() ){
         c.setRTCMemStatus();
         type = BootType::COLD_BOOT;
    }else{
         type = BootType::FIRST_BOOT;
    }

    return type;
}

void AppFrame::setUp()
{
    _appCtx._bootType = getBootType();

    for(int i=0;i < _rs;i++)
        _r[i]->setUp(&_appCtx);
}

void AppFrame::init()
{
    for(int i=0;i < _rs;i++)
        _r[i]->init();
}

void AppFrame::run()
{
    Msg event;
    int i;
    while(_eq.getQ(event)){
        for(i=0;i < _rs;i++){
            _r[i]->handleMsgIn(event);
        }
    }

    for(i=0;i < _rs;i++){
        _r[i]->executeAlways();
    }

    if( _appCtx._sleepNow ){
        for(i=0;i < _rs;i++){
            _r[i]->tearDown();
        }
        //Sleep
        ESP.deepSleep(2e6);
    }
}
