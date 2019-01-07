#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <Ticker.h>

#include "commondefs.h"

class Buttons: public runnable_t{
  public:
  Buttons(eQueue_t &eq,const uint8_t ioPin);

  void handleMsgIn(const Msg &msg);
  void executeAlways();
  void setUp();
  void init();
 
  private:
  enum class State{
    BTN_INIT,
    BTN_IS_DOWN,
    BTN_IS_UP
  };
  void timerCallBack();
  void checkBtn();
 
  Ticker  _tick;
  State   _st;
  uint8_t _ioPin;
  uint16_t _btnPressedTime;
  volatile uint16_t _settleTime;
  int   _ioState{0};

#ifdef DEBUG
    const char* _stName{nullptr};
#endif

  
};


#endif
