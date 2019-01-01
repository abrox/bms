#include "battery.h"
#include <math.h>

namespace bms {
// Values from :http://all-about-lead-acid-batteries.capnfatz.com/all-about-lead-acid-batteries/lead-acid-battery-maintenance/battery-voltmeter-soc-temperature-correction/
const float Battery::OCV_0   = 11.89;
const float Battery::OCV_25  = 12.06;
const float Battery::OCV_50  = 12.24;
const float Battery::OCV_75  = 12.45;
const float Battery::OCV_100 = 12.65;

Battery::Battery()
{

}


void  Battery::setContext(const BatteryCtx  &ctx)
{
     _ctx = ctx;
}

void  Battery::getContext(BatteryCtx  &ctx)
{
    ctx = _ctx;
}

int16_t Battery::initilize(const BatteryCfg &cfg)
{
    _cfg   = cfg;

    //Inialize to full state. setContext will update these to correct.
    _ctx.interval      = _cfg.interval;
    _ctx.maxCapasity   = _cfg.ratedCapasity*ONEHOUR;
    _ctx.releasableCur = _ctx.maxCapasity;

   return 0;
}
int16_t Battery::update(const Measurement_t &data)
{
    BattState state = BattState::IDLE;


    if( data.cu > _cfg.thresholdCurrent ){
       state = BattState::CHARGE;
    }else
     if( _cfg.thresholdCurrent + data.cu < 0 ){
        state = BattState::DISCHARGE;
    }else
     if(_ctx.timeInState < _cfg.ocvWaitTime){
        state = BattState::WAIT_TO_BE_IDLE;
    }

   _lastMeasurement = data;
   changeStateTo(state);

   return handleBatteryStates();
}
int16_t Battery::handleBatteryStates()
{

    int16_t rc = 0;
    float cu;
    int16_t cu2;
    switch (_ctx.state) {
    case BattState::UNKNOWN:
        break;
    case BattState::WAIT_TO_BE_IDLE:
        break;
    case BattState::IDLE:
        break;
    case BattState::CHARGE:
    case BattState::DISCHARGE:
        if(_ctx.state == BattState::CHARGE)
            cu = getIRCChargeCurrent();
        else
            cu  = getPCDischargeCurrent();

        cu += _ctx.cuLeft;
        _ctx.cuLeft = 0.0;
        cu2 = static_cast<float>(cu);
        _ctx.releasableCur += cu2 *_ctx.interval;
        _ctx.cuLeft = cu - cu2;

        break;
    default:
        rc=-1;
        break;
    }
    return rc;
}

void Battery::changeStateTo(const BattState state)
{
    if( _ctx.state == state ){
        _ctx.timeInState += _ctx.interval;

    }else{
       _ctx.state = state;
       _ctx.timeInState=0;
    }

}

int16_t Battery::getSOCbyOCV(const float &voltage,  const uint16_t &temp)
{
    float  volt = getTempCompensatedVoltage(voltage,temp);
    int16_t soc =-1;

    if ( volt <= OCV_0 ){
        soc = 0;
    }else
     if ( volt <= OCV_25 ){
        soc = getAddedProcent(OCV_0,OCV_25,volt);

    }else
     if ( volt <= OCV_50 ){
        soc = 25;
        soc += getAddedProcent(OCV_25,OCV_50,volt);

    }else
     if ( volt <= OCV_75 ){
         soc = 50;
         soc += getAddedProcent(OCV_50,OCV_75,volt);

    }else {
         soc = 75;
         soc += getAddedProcent(OCV_75,OCV_100,volt);
    }

    return soc;
}

float Battery::getTempCompensatedVoltage(const float &voltage, const int16_t &temp)
{
    //Magic for this come from page:
    //http://all-about-lead-acid-batteries.capnfatz.com/all-about-lead-acid-batteries/lead-acid-battery-maintenance/battery-voltmeter-soc-temperature-correction/
    return voltage + 0.0024*(temp-80);
}

int16_t Battery::getAddedProcent(const float &start,const float &end, const float &volt)
{
    float v = (end-start)/25;
    return (volt-start)/v;
}

float Battery::getPCDischargeCurrent()
{
    const Measurement_t &l = _lastMeasurement;
    const BatteryCfg      &c = _cfg;
    float cu = l.cu<0.0?-l.cu:l.cu;
    float val= pow(cu,c.peukert)/ pow((c.ratedCapasity/c.hourRate),(c.peukert-1));

    return  l.cu<0.0?-val:val;
}

float Battery::getIRCChargeCurrent()
{
    const Measurement_t &l = _lastMeasurement;
    const BatteryCfg      &c = _cfg;
    const BatteryCtx     &x =_ctx;

    //R0 = rInternal [ 1 + A0 ( 1 − SOC ) ]
    //rInternal= internal resistance when SOC =100
    //A0=constant
    float_t soc = (float_t)x.releasableCur/x.maxCapasity;
    float_t r = x.rInternal*(1.0+1.0*(1.0-soc));
    //I2=I-IxR0
    return l.cu-l.cu*r;
}
}//End of namespace
