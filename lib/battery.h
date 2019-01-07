#ifndef BATTERY_H
#define BATTERY_H
#include <inttypes.h>
#include "configdata.h"
#include "msg.h"

namespace bms {


class Battery
{

public:


    Battery();

    ///Set battery context.
    /// Restore battery state after sleep mode.
    ///
    void setContext(const BatteryCtx  &ctx);

    ///Get Battery context.
    /// Get battery state so that we can save it
    /// during deep sleep.
    ///
    void getContext(BatteryCtx  &ctx);

    ///Update measurement data.
    ///\return 0 in case success.
    ///
    int16_t update(const CurrentData &data);

    void getBatteryStats(BatteryStats& stats);

    int16_t getSOC(){return 0;}

    /// Intilize battery data.
    /// Assumption is that battery is full in this point-.
    /// In case not setContext will update correct state
    /// if saved succesfully earlier.
    ///
    int16_t initilize(const BatteryCfg &cfg);

private:

    ///Update state machine state and time in state.
    ///
    void changeStateTo(const BattState state);

    ///Get temperature conmpensated State Of Charge(SOC).
    ///By Open Circuit Voltage (OCV )
    ///\return SOC as persent 0-100
    ///
    int16_t getSOCbyOCV(const float &voltage, ///< Meassured voltage.
                        const uint16_t &temp  ///< Battery temperature as Farenheit.
                       );

    ///Make temperature compensation to meassured voltage.
    ///\return Reference voltage in 80 degree in farenheit.
    ///
    float getTempCompensatedVoltage(const float &voltage, ///< Meassured voltage.
                                    const int16_t &temp   ///< Battery temperature as Farenheit.
                                    );

    ///Calculate procents to be added.
    /// OCV voltage change is not linear trough 0-100% charge
    /// Thus calculate values for 25% slot
    /// \return added value from 0-25
    ///
    int16_t getAddedProcent(const float &start,///< Begin of range.
                            const float &end,  ///< end of range.
                            const float &v);   ///< Meassured value

    ///Handle actions relative for current state.
    ///
    int16_t handleBatteryStates();

    ///Get peukert's corrected current.
    ///Calculate corrected current.
    /// Source for Algoritm http://www.smartgauge.co.uk/calcs/peukert.xls
    ///
    float getPCDischargeCurrent();

    /// Calculate charge current.
    /// Try to take battery internal resistance in count.
    /// https://www.quora.com/How-do-I-distinguish-between-good-and-bad-12V-lead-acid-batteries-with-the-internal-resistance-value.
    /// \return
    ///
    float getIRCChargeCurrent();

    //If IR>30 milliohm, battery is in very bad condition. Probably unusable.
    //If IR is between 10 to 30 milliohm, still poor condition but may be usable or revivable.
    //If IR is between 5 to 10 milliohm, it is in good condition.
    //If IR is less than 5 milliohm, it is in very good condition.

    BatteryCfg     _cfg;
    BatteryCtx     _ctx;
    CurrentData _lastMeasurement; ///< Result of last measurement.

    static const float OCV_0;
    static const float OCV_25;
    static const float OCV_50;
    static const float OCV_75;
    static const float OCV_100;
};

}//end of namespace
#endif // BATTERY_H
