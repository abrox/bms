#include "batterymonitor.h"


BatteryMonitor::BatteryMonitor(eQueue_t &eq):Runnable(eq)
{

}
void BatteryMonitor::executeAlways()
{
   ;
}

void BatteryMonitor::handleMsgIn(const event_t &msg)
{


}

bool BatteryMonitor::setUp()
{

    return true;
}


