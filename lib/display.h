#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Ticker.h>
#include <U8g2lib.h>
#include <SPI.h>

#include "commondefs.h"
#include "ui.h"

typedef U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI U8g2;

class MyDisplay:public U8g2,public DisplayIf{
public:
   MyDisplay():U8g2(U8G2_R0, OLED_CLK, OLED_MOSI, OLED_CS,OLED_DC, OLED_RESET){;}
   void menuSelection(const char* title,const Menu& m,const uint8_t select=0);
   void YesNoQuestion(const char* title,const char* q,const boolean yes);
   virtual void showStats(const BatteryStats& st,const TimeElapsed& delta );
   virtual void showSOC(const CurrentData& cd,const uint8_t soc);
   virtual void show(const char* title, const char* str);

 private:
   uint16_t drawTitle(const char* title);

};

class Display: public runnable_t{
  public:
  Display(eQueue_t &eq):Runnable(eq){;}

  void handleMsgIn(const Msg &msg);
  void setUp();
 
  private:

  MenuCtrl * _ui{nullptr};
  MyDisplay u8g2;
  
};


#endif
