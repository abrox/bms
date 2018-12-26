#ifndef BATTERY_H
#define BATTERY_H
#include <inttypes.h>

namespace bms {

const int ONEHOUR=3600;

enum state_t{
    UNKNOWN,
    WAIT_TO_BE_IDLE,
    IDLE,
    CHARGE,
    DISCHARGE
};


/// Battery configuration data.
///
struct Config_t{
     uint32_t ocvWaitTime;      ///< Time to wait before meassure ocv 4h-24h default 8h ( sec )
     uint8_t  interval;         ///< Meassurement interval in sec.
     float    thresholdCurrent; ///< Minimun current to change from idle to charge / discharge.
     uint8_t  ratedCapasity;    ///< Battery size, default is 120 Ah
     uint8_t  hourRate;         ///< Battery at hour rate, default is 20.
     float    peukert;          ///< Peukert's Exponent. default is 1.15


     Config_t():ocvWaitTime(ONEHOUR*8),interval(1),thresholdCurrent(0.0001),
                ratedCapasity(120),hourRate(20),peukert(1.15){;}

};


///Dynamic system state.
///
struct Context_t{
    state_t  state;
    uint32_t timeInState; ///< Time in current state in sec.
    uint32_t interval;    /// Measurement interval in sec.
    uint32_t maxCapasity;
    uint32_t releasableCur;
    float cuLeft;       ///< Left over from previous round after full Asec removed.
    float rInternal; ///< Battery internal resistance mOhm, default is 5

    Context_t():state(UNKNOWN),timeInState(0),interval(0),maxCapasity(0),
                releasableCur(0),cuLeft(0.0),rInternal(0.005){}
};


class Battery
{

public:
    struct Measurement_t{
         float volt;    ///< meassured voltage.
         float cu;      ///<  meassured currect in(+) or out(-).
         int16_t temp;  ///<Battery temperature F in deg.
         Measurement_t(){}
         Measurement_t(const float &v,const float &c,int16_t const &t):
             volt(v),cu(c),temp(t){}
    };

    Battery();

    ///Set battery context.
    /// Restore battery state after sleep mode.
    ///
    void setContext(const Context_t  &ctx);

    ///Get Battery context.
    /// Get battery state so that we can save it
    /// during deep sleep.
    ///
    void getContext(Context_t  &ctx);

    ///Update measurement data.
    ///\return 0 in case success.
    ///
    int16_t update(const Measurement_t &data);

    int16_t getSOC(){return 0;}

    /// Intilize battery data.
    /// Assumption is that battery is full in this point-.
    /// In case not setContext will update correct state
    /// if saved succesfully earlier.
    ///
    int16_t initilize(const Config_t &cfg);

private:

    ///Update state machine state and time in state.
    ///
    void changeStateTo(const state_t state);

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

    Config_t      _cfg;
    Context_t     _ctx;
    Measurement_t _lastMeasurement; ///< Result of last measurement.

    static const float OCV_0;
    static const float OCV_25;
    static const float OCV_50;
    static const float OCV_75;
    static const float OCV_100;
};

}//end of namespace
#endif // BATTERY_H
