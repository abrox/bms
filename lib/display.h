#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Ticker.h>
#include <U8g2lib.h>
#include <SPI.h>

#include "commondefs.h"

class Display: public runnable_t{
  public:
  Display(eQueue_t &eq);

  void handleMsgIn(const event_t &msg);
  bool setUp();
 
  private:
  enum state{
    DISPLAY_OFF,
    DISPLAY_BATTERY,
    DISPLAY_WIFI,
    DISPLAY_MENU
  };
  void timerCallBack();
  void handleBatteryDisplay(const event_t &msg);
  void handleWifiDisplay(const event_t &msg);
  void handleMenuDisplay(const event_t &msg);
  void show(const event_t &msg);
 
  Ticker  _tick;
  state   _state;
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2;
  
};


#endif
