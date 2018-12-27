
#include "FS.h"
#include "configfileprovider.h"

#define NET_CONFIG_FILE PSTR("/config.json")

#define WLAN_STR PSTR("wlan")
#define WLAN_SSID_STR PSTR("ssid")
#define WLAN_PASS_STR PSTR("pass")

#define MQTT_STR PSTR("mqtt")
#define MQTT_ID_STR PSTR("id")
#define MQTT_IP_STR PSTR("ip")

#define NET_CONFIG_FILE PSTR("/config.json")

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

bool ConfigFileProvider::getMqttCfg(MqttCfg& cfg)
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



