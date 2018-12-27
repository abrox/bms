#include "FS.h"

#include "display.h"



void Display::timerCallBack(){
     _eq.putQ(MENU_TIMEOUT);
}

Display::Display(eQueue_t &eq):Runnable(eq),_state(DISPLAY_OFF),u8g2(U8G2_R0,  OLED_CLK, OLED_MOSI, OLED_CS,OLED_DC, OLED_RESET)
{
;
}

void Display::handleMsgIn(const event_t &msg)
{
    //Handle any state type messages.
    //Each display is responsible to set correct timeout.
    //So we can reset it allways when button is pressed.
    if( msg == BUTTON_DOWN ){
        u8g2.setPowerSave(false);
        _tick.detach();
    }else
     if( msg == MENU_TIMEOUT ){
         u8g2.setPowerSave(true);
         _state = DISPLAY_OFF;
    }
     
    switch(_state){
    case DISPLAY_OFF:
        if( msg == BUTTON_UP ){
            _state = DISPLAY_BATTERY;
        }else
         if( msg == BUTTON_DOWN_LONG ){
            _state = DISPLAY_WIFI;
        }else
         if( msg == BUTTON_DOWN_EXTLONG){
            _state = DISPLAY_MENU;
        }  
    break;
    case DISPLAY_BATTERY:
        if( msg == BUTTON_DOWN_LONG ){
            _state = DISPLAY_WIFI;
        }
    break;
    case DISPLAY_WIFI:
        if( msg == BUTTON_DOWN_EXTLONG){
            _state = DISPLAY_MENU;
        }
    break;
    }
     
    show(msg);    
}

bool Display::setUp()
{
  u8g2.begin();
  return true;
}

void Display::handleBatteryDisplay(const event_t &msg)
{
  if( msg == BUTTON_UP ){
     _tick.once(20, std::bind(&Display::timerCallBack, this));
     
   u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB12_tf);
    u8g2.setCursor(0,15);
    u8g2.print("Hello from handleBatteryDisplay");
 /*  
    u8g2.setFont(u8g2_font_ncenB12_tf);
    u8g2.setCursor(0,15);
    u8g2.print("C:");
    u8g2.print(monitor.shuntCurrent() * 1000, 4);
    u8g2.setCursor(0,35);
    u8g2.print("V:");
    u8g2.print(monitor.busVoltage(), 4);
    
    u8g2.setCursor(0,50);
    u8g2.print("T:");
    u8g2.print(temp, 2);
   */
   ; 
    
  } while ( u8g2.nextPage() );
  }
  
}
void Display::handleWifiDisplay(const event_t &msg)
{
   PGM_P head = PSTR("WIFI");
   PGM_P  str = PSTR("Lease btn to connect");
   bool show = false;
   
   if( msg == BUTTON_UP )
       _eq.putQ(START_MQTT);
   else
   if( msg == BUTTON_DOWN_LONG ){
       show = true;   
   }
   else
   if( msg == UPDATE_STATUS_DISLAY ){  
       show = true;
       str = _appCtx->_statusMsg;
   }else
   if( msg == MQTT_FAILED ||
       msg == MQTT_RUNNING){
       _tick.once(5, std::bind(&Display::timerCallBack, this));
   }

   if( show ){
       u8g2.firstPage();
       do {
           u8g2.setFont(u8g2_font_ncenB12_tf);
           u8g2.setCursor(0,15);
           u8g2.print(head);
           
           u8g2.setFont(u8g2_font_helvR08_tf);
           u8g2.setCursor(0,30);
           u8g2.print(str);
       } while ( u8g2.nextPage() );
    
   }
}
void Display::handleMenuDisplay(const event_t &msg)
{
  PGM_P head = PSTR("MENU");
   PGM_P  str = PSTR("Lease button");
   bool show = false;
   
   if( msg == BUTTON_UP ){
        str = PSTR("Delete netconfig");
        show = true;
   }
   else
   if( msg == BUTTON_DOWN_TWO_SEC ){
       //SPIFFS.remove(NET_CONFIG_FILE);
       str = PSTR("Config deleted");
       show = true;
   }
   else
   if( msg == BUTTON_DOWN_EXTLONG ){
       show = true;   
   }

   if( show ){
       u8g2.firstPage();
       do {
           u8g2.setFont(u8g2_font_ncenB12_tf);
           u8g2.setCursor(0,15);
           u8g2.print(head);
           
           u8g2.setFont(u8g2_font_helvR08_tf);
           u8g2.setCursor(0,30);
           u8g2.print(str);
       } while ( u8g2.nextPage() );
    
   }
  
}

 
void Display::show(const event_t &msg){

  switch( _state ){
    case DISPLAY_BATTERY:
        handleBatteryDisplay(msg);
    break;
    case DISPLAY_WIFI:
        handleWifiDisplay(msg);
    break;
    case DISPLAY_MENU:
        handleMenuDisplay(msg);
    break;
  }
}
