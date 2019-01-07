#include "FS.h"

#include "display.h"

uint16_t MyDisplay::drawTitle(const char* title)
{
  setFont(u8g2_font_ncenB12_tr);
  auto height   = getMaxCharHeight();
  auto startPos = getDisplayWidth()/2;
  startPos -= getStrWidth(title)/2;
  setCursor(startPos, height);
  print(title);
  return height;
}

void MyDisplay::menuSelection(const char* title,const Menu& m,const uint8_t select){
  firstPage();
  do {

    auto yPos = drawTitle(title);
    setFont(u8g2_font_helvR08_tf);
    auto height = getMaxCharHeight();

    yPos += height;
    auto row=1;

    for(auto const &s:m){
      if( row == select){
          drawBox(0,yPos-height+2,getStrWidth(s)+2,height+2);
          setDrawColor(0);
      }
      setCursor(0, yPos);
      print(s);
      setDrawColor(1);
      yPos+=height;
      row++;
    }
  }while ( nextPage() );
}

void MyDisplay::YesNoQuestion(const char* title,const char* q,const boolean yes)
{
  firstPage();
  do {
    auto yPos = drawTitle(title);
    setFont(u8g2_font_helvR08_tf);
    auto height = getMaxCharHeight();

    yPos += height;
    setCursor(0, yPos);
    print(q);
    const char* y= "YES ";
    const char* n= " NO ";
    yPos = getDisplayHeight();
    auto startPos = getDisplayWidth()/2;
    startPos -= getStrWidth(y)/2;
    startPos -= getStrWidth(n)/2;
    setCursor(startPos, yPos);
    if(yes){
       drawBox(startPos,yPos-height+2,getStrWidth(y)+2,height+2);
       setDrawColor(0);
       print(y);
       setDrawColor(1);
       print(n);
    }else{
       drawBox(startPos+getStrWidth(y),yPos-height+2,getStrWidth(n)+2,height+2);
       print(y);
       setDrawColor(0);
       print(n);
       setDrawColor(1);
     }
  }while ( nextPage() );
}

void MyDisplay::showStats(const BatteryStats& st,const TimeElapsed& te )
{
    auto hPos=0;
    firstPage();
    do {
     setFont(u8g2_font_helvR08_tf);
     hPos = getMaxCharHeight();
     setCursor(0,hPos);
     print(te.days);
     print("d:");
     print(te.hours);
     print("h:");
     print(te.mins);
     print("m");

     hPos += getMaxCharHeight();
     setCursor(0,hPos);
     print("SOC:");
     print(st.soc);
     print("%");
     hPos += getMaxCharHeight();;
     setCursor(0,hPos);
     print("SOH:");
     print(st.soh);
     print("%");
    } while ( nextPage() );
}

void MyDisplay::show(const char* title, const char* str){

    firstPage();
    do {
        auto yPos = drawTitle(title);
        setFont(u8g2_font_helvR08_tf);
        yPos += getMaxCharHeight();
        setCursor(0,yPos);
        print(str);
    } while ( nextPage() );
}

void MyDisplay::showSOC(const CurrentData& cd,const uint8_t soc)
{
    auto width = getDisplayWidth()-1;
    auto hPos = 15;
    auto vPos = 0;

   firstPage();
    do{
        //Draw screen with bar to show SOC
        drawBox(0,0,width*soc/100,hPos);
        hPos+=1;

        //Scale it 25,50,75,1nd 100
        drawHLine(0,hPos,width);
        for(auto i=0; i<=100;i++){
            if(!(i%25))
               drawVLine((i*width)/100,hPos,2);
        }

        //Split screen
        drawVLine(width/2,hPos,getDisplayHeight()-hPos);
        //Print heaaders
        setFont(u8g2_font_courR08_tr);
        setCursor(0,hPos+3+getMaxCharHeight());
        print("Voltage");
        setCursor(width/2+3,hPos+3+getMaxCharHeight());
        print("Current");

        //Print arrow up or down depend are we charge or discharge
        setFont(u8g2_font_unifont_t_symbols);
        if( cd.cu < 0 )
            drawGlyph(width-16,hPos+getMaxCharHeight(), 0x23f7);
        else
            drawGlyph(width-16,hPos+getMaxCharHeight(), 0x23f6);


        setFont(u8g2_font_courR14_tr);
        auto w = getStrWidth("88.88");
        //Put cursor to middle
        hPos   = getDisplayHeight();
        vPos   = width/4 - w/2-2;
        setCursor(vPos,hPos);

        print(cd.volt,2);

        //Put cursor to middle
        vPos  = width/2;
        vPos += vPos/2-w/2;
        setCursor(vPos,hPos);

        print(fabsf(cd.cu));

    }while (nextPage() );
}


void Display::handleMsgIn(const Msg &msg)
{
    //Handle any state type messages.
    //Each display is responsible to set correct timeout.
    //So we can reset it allways when button is pressed.
    if( msg == Msg::BUTTON_DOWN ){
       u8g2.setPowerSave(false);
        //99.9% boot up's menu is not needed,thus create when required.
        //even it's agains embedded we created it like this.
        //Note also that it's newer deleted, that's also planned action.
        if( !_ui )
           _ui = new MenuCtrl(_appCtx,&u8g2);
    }else
     if( msg == Msg::DISPLAY_OFF_TIMEOUT ){
        u8g2.setPowerSave(true);
    }

    if( _ui ){
        _ui->handleMsg(msg);
    }

}

void Display::setUp()
{
   u8g2.begin();
}
