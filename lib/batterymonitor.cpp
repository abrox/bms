#include "batterymonitor.h"
#include "configmanager.h"

BatteryMonitor::BatteryMonitor(eQueue_t &eq):Runnable(eq)
{

}
void BatteryMonitor::executeAlways()
{
   ;
}

void BatteryMonitor::handleMsgIn(const Msg &msg)
{
    if( msg == Msg::SOC_UPDATE ){
        _bat.update(_appCtx->_currentData);
        _appCtx->_soc = _bat.getSOC();
    }
}

void BatteryMonitor::setUp()
{
    ConfigManager& c=ConfigManager::getInstance();

    c.getBatteryCfg(_cfg);
    _bat.initilize(_cfg);

    if( c.getBatteryCtx(_ctx) )
        _bat.setContext(_ctx);
    else
        ;//todo
}
void BatteryMonitor::tearDown()
{
    ConfigManager& c=ConfigManager::getInstance();

    _bat.getContext(_ctx);
    c.saveBatCtxToRTCMem(_ctx);
}


