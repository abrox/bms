

#include "mqttclient.h"


void MqttClient::timerCallBack(){
     _eq.putQ(MQTT_CLIENT_TIMEOUT);
}

MqttClient::MqttClient(eQueue_t &eq):Runnable(eq),_state(IDLE),_reTryCount(0)
{;}

void MqttClient::executeAlways()
{
    _client.loop();
    delay(10);
    if ( _state == CONNECTED ){
    
         if( !_client.connected() ) {
             DPRINTLN(PSTR("Connection lost"));
             _reTryCount = _maxRetry;
             _state = WAIT_WIFI;
         }
    }
}
void MqttClient::handleMsgIn(const event_t &msg)
{
    switch(_state){
       case IDLE:
          if( msg == START_MQTT){
            
              if( readConfigFromFlash() ){
                  WiFi.begin(_cfg._ssid, _cfg._wifiPass);
                  _client.begin(_cfg._mqttServ, 1883, _net);
                  _tick.attach(1, std::bind(&MqttClient::timerCallBack, this));
                  _appCtx->updateStatusDisplay(PSTR("Starting WIFI..."));
                  _reTryCount = _maxRetry;
                  _state = WAIT_WIFI;
              }else{
                  _appCtx->updateStatusDisplay(PSTR("Missing or Invalid config"));
                  _eq.putQ(MQTT_FAILED);
              }
          }
       break;
       case WAIT_WIFI:
       if ( msg == MQTT_CLIENT_TIMEOUT ){
        
            if( WiFi.status() == WL_CONNECTED ){
                _client.connect(_cfg._mqttId);
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
                   _client.connect(_cfg._mqttId);
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
  return true;
}

void MqttClient::init()
{
  
}

bool MqttClient::readConfigFromFlash()
{
  ConfigFileProvider& c=ConfigFileProvider::getInstance();
  return(c.getMqttCfg(_cfg));
}
