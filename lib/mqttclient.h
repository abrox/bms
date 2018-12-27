#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <Ticker.h>

#include "commondefs.h"
#include "configfileprovider.h"


class MqttClient: public runnable_t{
  public:
  MqttClient(eQueue_t &eq);

  void handleMsgIn(const event_t &msg);
  void executeAlways();
  bool setUp();
  void init();
 
  private:
  enum state{
    IDLE,
    WAIT_WIFI,
    WAIT_BROKER,
    CONNECTED
  };
  bool readConfigFromFlash();

  void timerCallBack();
  
  bool reTry(){
    if( _reTryCount )
        _reTryCount--;
        
    return _reTryCount > 0;
  }
  Ticker  _tick;
  state   _state;
  WiFiClient _net;
  MQTTClient _client;
  const uint8_t _maxRetry = 20;
  uint8_t     _reTryCount;
  MqttCfg  _cfg;

};


#endif
