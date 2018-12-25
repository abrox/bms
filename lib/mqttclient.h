#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#include "commondefs.h"

class MqttClient: public runnable_t{
  public:
  MqttClient(eQueue_t &eq);

  void handleMsgIn(const event_t &msg);
  void executeAlways();
  bool setUp();
  void init();
 
  private:
  enum state{
    CONFIG_MISSING,
    WAIT_CONFIG,
    IDLE,
    WAIT_WIFI,
    WAIT_BROKER,
    CONNECTED
  };
  bool readConfigFromFlash();
  bool readConfigFromSerial();
  
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

  const size_t _confBuffSize = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 130;
  const uint8_t _maxRetry = 20;
  
  DynamicJsonBuffer _jsonBuffer;
  const char* _ssid;
  const char* _wifiPass;
  const char* _mqttId;
  const char* _mqttServ;
  uint8_t     _reTryCount;
  

};


#endif
