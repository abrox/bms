#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "commondefs.h"
#include "RTClib.h"

typedef struct {
        const char* _ssid{NULL};
        const char* _wifiPass{NULL};
        const char* _mqttId{NULL};
        const char* _mqttServ{NULL};
}MqttCfg;

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

        typedef struct {
          uint32_t crc32;
          uint32_t timestamp;
        } rtcData_t;

        rtcData_t _rtcData;
        const size_t _confBuffSize = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 130;
        DynamicJsonBuffer _jsonBuffer{_confBuffSize};

    public:
        bool configFilesExist();
        bool getRTCMemStatus();
        bool setRTCMemStatus();
        bool getMqttCfg(MqttCfg& cfg);
        ConfigManager(ConfigManager const&)  = delete;
        void operator=(ConfigManager const&)  = delete;

        // Note: Scott Meyers mentions in his Effective Modern
        //       C++ book, that deleted functions should generally
        //       be public as it results in better error messages
        //       due to the compilers behavior to check accessibility
        //       before deleted status
};

#endif // CONFIGFILEPROVIDER_H
