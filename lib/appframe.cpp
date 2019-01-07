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

void AppFrame::AppFrame::initAppCtx(AppCtx& _appCtx)
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

    _appCtx._startTime = c.getStartTime();
    _appCtx._bootType  = type;
}

void AppFrame::setUp()
{
    initAppCtx(_appCtx);

    for(auto& r:_r)
        r->setUp(&_appCtx);
}

void AppFrame::init()
{
    for(auto& r:_r)
        r->init();
}

void AppFrame::run()
{
    Msg event;

    while(_eq.getQ(event)){
        for(auto& r:_r){
            r->handleMsgIn(event);
        }
    }

    for(auto& r:_r){
        r->executeAlways();
    }

    if( _appCtx._sleepNow ){
        for(auto& r:_r){
            r->tearDown();
        }
        //Sleep
        ESP.deepSleep(2e6);
    }
}
