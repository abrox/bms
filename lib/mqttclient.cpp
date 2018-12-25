
#include "FS.h"

#include "mqttclient.h"

#define WLAN_STR PSTR("wlan")
#define WLAN_SSID_STR PSTR("ssid")
#define WLAN_PASS_STR PSTR("pass")

#define MQTT_STR PSTR("mqtt")
#define MQTT_ID_STR PSTR("id")
#define MQTT_IP_STR PSTR("ip")

#define NET_CONFIG_FILE PSTR("/config.json")


void MqttClient::timerCallBack(){
     _eq.putQ(MQTT_CLIENT_TIMEOUT);
}

MqttClient::MqttClient(eQueue_t &eq):Runnable(eq),_state(WAIT_WIFI),_jsonBuffer(_confBuffSize),
_ssid(NULL),_wifiPass(NULL), _mqttId(NULL),_mqttServ(NULL),_reTryCount(0)
{;}

void MqttClient::executeAlways()
{
    _client.loop();
    
    if ( _state == CONNECTED ){
    
         if( !_client.connected() ) {
             DPRINTLN(PSTR("Connection lost"));
             _state = WAIT_WIFI;
         }
    }else
     if( _state == WAIT_CONFIG ){

         if( readConfigFromSerial() ){
             _tick.detach();
             _appCtx->updateStatusDisplay(PSTR("MQTT config Ok, starting Mqtt.."));
             _eq.putQ(START_MQTT);
             _state = IDLE;
         }
    }
}
void MqttClient::handleMsgIn(const event_t &msg)
{
    switch(_state){
       case CONFIG_MISSING:
           if( msg == START_MQTT){
            
               _tick.attach(30, std::bind(&MqttClient::timerCallBack, this));
               _appCtx->updateStatusDisplay(PSTR("Wait 30sec mqtt config from serial"));
               _state = WAIT_CONFIG;
           }
       break;
       case WAIT_CONFIG:
           if ( msg == MQTT_CLIENT_TIMEOUT ){
                _tick.detach();
                _appCtx->updateStatusDisplay(PSTR("No config received"));
                _state = CONFIG_MISSING;
                _eq.putQ(MQTT_FAILED);
       }
       break;
       case IDLE:
          if( msg == START_MQTT){
            
              if( readConfigFromFlash() ){
                  WiFi.begin(_ssid, _wifiPass);
                  _client.begin(_mqttServ, 1883, _net);
                  _tick.attach(1, std::bind(&MqttClient::timerCallBack, this));
                  _appCtx->updateStatusDisplay(PSTR("Starting WIFI..."));
                  _reTryCount = _maxRetry;
                  _state = WAIT_WIFI;
              }else{
                  _appCtx->updateStatusDisplay(PSTR("Missing or Invalid config"));
                  _state = CONFIG_MISSING;
              }
          }
       break;
       case WAIT_WIFI:
       if ( msg == MQTT_CLIENT_TIMEOUT ){
        
            if( WiFi.status() == WL_CONNECTED ){
                _client.connect(_mqttId);
                _appCtx->updateStatusDisplay(PSTR("Wifi ok, connecting to Broker"));
                _state = WAIT_BROKER;
            }else{
                if( !reTry() ){
                   _appCtx->updateStatusDisplay(PSTR("Network not found"));
                   _tick.detach();
                   _state = IDLE;
                   _eq.putQ(MQTT_FAILED);
                }else
                   _appCtx->updateStatusDisplay(PSTR("Searching network..."));
            }
       }
      break;
      case WAIT_BROKER:
      if ( msg == MQTT_CLIENT_TIMEOUT){
        
           if(_client.connected()){
               _appCtx->updateStatusDisplay(PSTR("Mqtt ok"));
               _eq.putQ(MQTT_RUNNING);
               _state = CONNECTED;
            
           }else{
                if( !reTry() ){
                    _appCtx->updateStatusDisplay(PSTR("Broker not found"));
                    _tick.detach();
                    _state = IDLE;
                    _eq.putQ(MQTT_FAILED);
                }else{
                   _appCtx->updateStatusDisplay(PSTR("Try to connect broker"));
                   _client.connect(_mqttId);
                }
           }
      }
      break;
      case CONNECTED:
      if ( msg == MQTT_CLIENT_TIMEOUT){
        
          _client.publish("/hello", "world");
      }else
      if( msg == START_MQTT){
          _appCtx->updateStatusDisplay(PSTR("Mqtt already running"));
          _eq.putQ(MQTT_RUNNING);
      }
      break;
    }
}

bool MqttClient::setUp()
{

  if( SPIFFS.exists(NET_CONFIG_FILE)){
    _state = IDLE;
  }else
     _state = CONFIG_MISSING;
     
  return true;
}

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
bool MqttClient::readConfigFromFlash()
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

    _ssid     = root["wlan"]["ssid"];
    _wifiPass = root["wlan"]["pass"];
    _mqttId   = root["mqtt"]["id"];
    _mqttServ = root["mqtt"]["ip"];
  
  return true;
}

void MqttClient::init()
{
  
}
