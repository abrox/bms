#ifndef CONFIGDATA_H
#define CONFIGDATA_H


struct MqttCfg{
    const char* _ssid{nullptr};
    const char* _wifiPass{nullptr};
    const char* _mqttId{nullptr};
    const char* _mqttServ{nullptr};
};

const int ONEHOUR=3600;
/// Battery configuration data.
///
struct BatteryCfg{
    uint32_t ocvWaitTime;      ///< Time to wait before meassure ocv 4h-24h default 8h ( sec )
    uint8_t  interval;         ///< Meassurement interval in sec.
    float    thresholdCurrent; ///< Minimun current to change from idle to charge / discharge.
    uint8_t  ratedCapasity;    ///< Battery size, default is 120 Ah
    uint8_t  hourRate;         ///< Battery at hour rate, default is 20.
    float    peukert;          ///< Peukert's Exponent. default is 1.15


    BatteryCfg():ocvWaitTime(ONEHOUR*8),interval(1),thresholdCurrent(0.0001),
        ratedCapasity(120),hourRate(20),peukert(1.15){;}

};


enum class BattState{
    UNKNOWN,
    WAIT_TO_BE_IDLE,
    IDLE,
    CHARGE,
    DISCHARGE
};


struct RtcData{
    uint32_t crc32;
    uint32_t timestamp;
} ;

static_assert(sizeof(RtcData)%4==0,"RTC Memory alligment problem");

///Dynamic system state.
///
struct BatteryCtx{
    BattState  state;
    uint32_t timeInState; ///< Time in current state in sec.
    uint32_t interval;    /// Measurement interval in sec.
    uint32_t maxCapasity;
    uint32_t releasableCur;
    float cuLeft;       ///< Left over from previous round after full Asec removed.
    float rInternal; ///< Battery internal resistance mOhm, default is 5

    BatteryCtx():state(BattState::UNKNOWN),timeInState(0),interval(0),maxCapasity(0),
        releasableCur(0),cuLeft(0.0),rInternal(0.005){}
};

static_assert(sizeof(BatteryCtx)%4==0,"RTC Memory alligment problem");
#endif
