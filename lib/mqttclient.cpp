

#include "mqttclient.h"
#include "configmanager.h"

void MqttClient::timerCallBack(){
    _eq.putQ(Msg::MQTT_CLIENT_TIMEOUT);
}

MqttClient::MqttClient(eQueue_t &eq):Runnable(eq),_state(State::IDLE),_reTryCount(0)
{;}

void MqttClient::executeAlways()
{
    _client.loop();
    delay(10);
    if ( _state == State::CONNECTED ){

        if( !_client.connected() ) {
            DPRINTLN(PSTR("Connection lost"));
            _reTryCount = _maxRetry;
            _state = State::WAIT_WIFI;
        }
    }
}
void MqttClient::handleMsgIn(const Msg &msg)
{
    switch(_state){
    case State::IDLE:
        if( msg == Msg::START_MQTT){
            
            if( readConfigFromFlash() ){
                WiFi.begin(_cfg._ssid, _cfg._wifiPass);
                _client.begin(_cfg._mqttServ, 1883, _net);
                _tick.attach(1, std::bind(&MqttClient::timerCallBack, this));
                _appCtx->updateStatusDisplay(PSTR("Starting WIFI..."));
                _reTryCount = _maxRetry;
                _state = State::WAIT_WIFI;
            }else{
                _appCtx->updateStatusDisplay(PSTR("Missing or Invalid config"));
                _eq.putQ(Msg::MQTT_FAILED);
            }
        }
        break;
    case State::WAIT_WIFI:
        if ( msg == Msg::MQTT_CLIENT_TIMEOUT ){

            if( WiFi.status() == WL_CONNECTED ){
                _client.connect(_cfg._mqttId);
                _appCtx->updateStatusDisplay(PSTR("Wifi ok, connecting to Broker"));
                _state = State::WAIT_BROKER;
            }else{
                if( !reTry() ){
                    _appCtx->updateStatusDisplay(PSTR("Network not found"));
                    _tick.detach();
                    _state = State::IDLE;
                    _eq.putQ(Msg::MQTT_FAILED);
                }else
                    _appCtx->updateStatusDisplay(PSTR("Searching network..."));
            }
        }
        break;
    case State::WAIT_BROKER:
        if ( msg == Msg::MQTT_CLIENT_TIMEOUT){

            if(_client.connected()){
                _appCtx->updateStatusDisplay(PSTR("Mqtt ok"));
                _eq.putQ(Msg::MQTT_RUNNING);
                _state = State::CONNECTED;

            }else{
                if( !reTry() ){
                    _appCtx->updateStatusDisplay(PSTR("Broker not found"));
                    _tick.detach();
                    _state = State::IDLE;
                    _eq.putQ(Msg::MQTT_FAILED);
                }else{
                    _appCtx->updateStatusDisplay(PSTR("Try to connect broker"));
                    _client.connect(_cfg._mqttId);
                }
            }
        }
        break;
    case State::CONNECTED:
        if ( msg == Msg::MQTT_CLIENT_TIMEOUT){

            _client.publish("/hello", "world");
        }else
            if( msg == Msg::START_MQTT){
                _appCtx->updateStatusDisplay(PSTR("Mqtt already running"));
                _eq.putQ(Msg::MQTT_RUNNING);
            }
        break;
    }
}

void MqttClient::setUp()
{    
    ;
}

void MqttClient::init()
{

}

bool MqttClient::readConfigFromFlash()
{
    ConfigManager& c=ConfigManager::getInstance();
    return(c.getMqttCfg(_cfg));
}
