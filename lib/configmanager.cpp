
#include "FS.h"
#include "configmanager.h"

#define NET_CONFIG_FILE PSTR("/config.json")

#define WLAN_STR PSTR("wlan")
#define WLAN_SSID_STR PSTR("ssid")
#define WLAN_PASS_STR PSTR("pass")

#define MQTT_STR PSTR("mqtt")
#define MQTT_ID_STR PSTR("id")
#define MQTT_IP_STR PSTR("ip")

#define NET_CONFIG_FILE PSTR("/config.json")


///Utility function to calculate CRC.
///
uint32_t calculateCRC32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xffffffff;
    while (length--) {
        uint8_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1) {
            bool bit = crc & 0x80000000;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x04c11db7;
            }
        }
    }
    return crc;
}

void printDateTime(const DateTime& dt)
{
    DPRINT(dt.year(), DEC);
    DPRINT('/');
    DPRINT(dt.month(), DEC);
    DPRINT('/');
    DPRINT(dt.day(), DEC);
    DPRINT(' ');
    DPRINT(dt.hour(), DEC);
    DPRINT(':');
    DPRINT(dt.minute(), DEC);
    DPRINT(':');
    DPRINTLN(dt.second(), DEC);
}

#if 0
bool MqttClient::readConfigFromSerial()
{
    bool  rc = false;
    const char* msg = NULL;

    _jsonBuffer.clear();
    JsonObject& root = _jsonBuffer.parseObject(Serial);

    //Validate JSON before store it to flash.
    //todo: Some kind of shema& validator would be cool...
    //This is bit clumsy way
    if( !root.success() ){
        msg = PSTR("failed to read from serial");
    }else{
        ///////////WLAN///////////////////
        if( !root.containsKey(WLAN_STR) ){
            msg = WLAN_STR;
        }else{
            JsonObject& wlan = root[WLAN_STR];
            if( !wlan.containsKey(WLAN_SSID_STR) ){
                msg = WLAN_SSID_STR;
            }else
                if( !wlan.containsKey(WLAN_PASS_STR) ){
                    msg = WLAN_PASS_STR;
                }else{
                    ///////////MQTT///////////////////
                    if( !root.containsKey(MQTT_STR) ){
                        msg = MQTT_STR;
                    }else{
                        JsonObject& mqtt = root[MQTT_STR];
                        if( !mqtt.containsKey(MQTT_ID_STR) ){
                            msg = MQTT_ID_STR;
                        }else
                            if( !mqtt.containsKey(MQTT_IP_STR) ){
                                msg = MQTT_IP_STR;
                            }else{
                                msg = PSTR("Config received");
                                File configFile = SPIFFS.open(NET_CONFIG_FILE, "w");
                                if( !configFile ) {
                                    msg = PSTR("Failed to open config file for writing");
                                }else{
                                    root.printTo(configFile);
                                    msg = PSTR("Config received and saved");
                                    rc = true;
                                }//Saved ok
                            }// All required fields exist
                    }//root.containsKey(MQTT_STR)
                }//all wlan fiels exist
        }//root.containsKey(WLAN_STR)
    }//root ok

    _appCtx->updateStatusDisplay(msg);

    return rc;
}
#endif

bool ConfigManager::getMqttCfg(MqttCfg& cfg)
{

    File configFile = SPIFFS.open(NET_CONFIG_FILE, "r");
    if( !configFile ){
        DPRINTLN(PSTR("Failed to open config file"));
        return false;
    }

    size_t size = configFile.size();
    if( size > _confBuffSize ){
        DPRINTLN(PSTR("Config file size is too large"));
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    _jsonBuffer.clear();
    JsonObject& root = _jsonBuffer.parseObject(buf.get());

    if( !root.success() ){
        DPRINTLN(PSTR("Failed to parse config file"));
        return false;
    }

#ifdef DEBUG
    root.printTo(Serial);
#endif

    //Todo config version validity check
    //int rev_maj = root["rev"]["maj"]; // 1
    //int rev_min = root["rev"]["min"]; // 1

    cfg._ssid     = root["wlan"]["ssid"];
    cfg._wifiPass = root["wlan"]["pass"];
    cfg._mqttId   = root["mqtt"]["id"];
    cfg._mqttServ = root["mqtt"]["ip"];

    return true;
}

bool ConfigManager::getRTCMemStatus()
{
    bool rc = false;

    // Read struct from RTC memory
    if( ESP.rtcUserMemoryRead(0, (uint32_t*)&_rtcData, sizeof(RtcData)) ) {
        uint32_t crcOfData = calculateCRC32((uint8_t*) &_rtcData.timestamp, sizeof(_rtcData.timestamp));
        DPRINT("CRC32 of data: ");
        DPRINTLN(crcOfData, HEX);
        DPRINT("CRC32 read from RTC: ");
        DPRINTLN(_rtcData.crc32, HEX);

        if( crcOfData != _rtcData.crc32 ) {
            DPRINTLN("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
        } else {
            DPRINTLN("CRC32 check ok, data is probably valid.");
            DPRINT("Date time: ");
            printDateTime(DateTime(_rtcData.timestamp));
            DPRINT("Now: ");
            printDateTime(rtc.now());
            rc = true;
        }
    }
    return rc;
}
bool ConfigManager::setRTCMemStatus()
{
    bool rc =false;
    DateTime now = rtc.now();
    _rtcData.timestamp = now.unixtime();
    _rtcData.crc32 = calculateCRC32((uint8_t*) &_rtcData.timestamp, sizeof(_rtcData.timestamp));

    if( ESP.rtcUserMemoryWrite(0, (uint32_t*) &_rtcData, sizeof(RtcData)) ) {
        DPRINT("CRC32: ");
        DPRINTLN(_rtcData.crc32, HEX);
        DPRINT("Date time: ");
        printDateTime(now);
        rc = true;
    }

    return rc;
}


bool ConfigManager::configFilesExist()
{
    bool rc= false;
    if( SPIFFS.exists(NET_CONFIG_FILE))
        rc=true;

    return rc;
}
bool ConfigManager::getBatteryCfg(BatteryCfg& cfg)
{
    return false;
}

bool ConfigManager::getBatteryCtx(BatteryCtx& ctx)
{
    bool rc = false;
    uint32_t crc;

    if( ESP.rtcUserMemoryRead(_batCtxOffset, (uint32_t*)&crc, sizeof(uint32_t)) ) {
       ;
    }
    if( ESP.rtcUserMemoryRead(_batCtxOffset+1, (uint32_t*)&ctx, sizeof(BatteryCtx)) ) {

        uint32_t crcOfData = calculateCRC32((uint8_t*) &ctx, sizeof(BatteryCtx));

        DPRINT("BatteryCtx CRC32: ");
        DPRINTLN(crcOfData, HEX);

        DPRINT(" From RTC CRC32: ");
        DPRINTLN(crcOfData, HEX);

        if( crcOfData == crc )
            rc=true;
        else
            rc = false;
   }

    return rc;
}

bool ConfigManager::saveBatCtxToRTCMem(BatteryCtx& ctx)
{
    bool rc = false;
    uint32_t crc = calculateCRC32((uint8_t*) &ctx, sizeof(BatteryCtx));

    if( ESP.rtcUserMemoryWrite(_batCtxOffset, (uint32_t*) &crc, sizeof(uint32_t)) ) {
        bool rc = true;
        DPRINT("Save BatteryCtx... CRC");
        DPRINTLN(crc, HEX);
    }

    if( ESP.rtcUserMemoryWrite(_batCtxOffset+1, (uint32_t*) &ctx, sizeof(BatteryCtx)) ) {
        bool rc = true;
    }

    return rc;
}
