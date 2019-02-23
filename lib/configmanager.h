#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "RTClib.h"

#include "commondefs.h"
#include "configdata.h"



class ConfigManager
{
public:
    static ConfigManager& getInstance()
    {
        static ConfigManager    instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

private:
    ConfigManager() {;}


    RtcData _rtcData;
    const size_t _confBuffSize = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 130;
    DynamicJsonBuffer _jsonBuffer{_confBuffSize};

    const size_t _batCtxOffset = sizeof(RtcData)/4;

public:
    bool configFilesExist();
    bool getRTCMemStatus();
    bool setRTCMemStatus();
    bool getMqttCfg(MqttCfg& cfg);
    bool getBatteryCfg(BatteryCfg& cfg);
    bool getBatteryCtx(BatteryCtx& ctx);
    bool saveBatCtxToRTCMem(BatteryCtx& ctx);

    ///Read current sensor and shunt configuration.
    /// \return True when success False if configuration not exist.
    ///
    bool getCurrentSensorCfg(CurrentCfg& cfg ///< [OUT] Updated cofiguration.
                             );

    uint32_t getStartTime(){return _rtcData.timestamp;}

    ConfigManager(ConfigManager const&)  = delete;
    void operator=(ConfigManager const&)  = delete;

    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status
};

#endif // CONFIGFILEPROVIDER_H
