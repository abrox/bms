#include "buttons.h"



void Buttons::timerCallBack(){
    _btnPressedTime++;

    if( _btnPressedTime == 2 )
        _eq.putQ(Msg::BUTTON_DOWN_TWO_SEC);
    if( _btnPressedTime == 3 )
        _eq.putQ(Msg::BUTTON_DOWN_LONG);
    else
        if( _btnPressedTime == 6 )
            _eq.putQ(Msg::BUTTON_DOWN_EXTLONG);
}

Buttons::Buttons(eQueue_t &eq,const uint8_t ioPin):Runnable(eq),_state(State::BTN_UNKNOWN),_ioPin(ioPin),_btnPressedTime(0),_filter(0)
{
    ;
}

void Buttons::handleMsgIn(const Msg &msg)
{
    ;
}

void Buttons::setUp()
{

    //Setup menu button
    pinMode(_ioPin, INPUT_PULLUP);
}

void Buttons::executeAlways()
{
    if( _filter )
        _filter--;
    else
        _filter = checkBtn();
    
}

uint16_t Buttons::checkBtn()
{
    uint16_t filterVal=0;
    switch(_state){
    case State::BTN_IS_DOWN:
        if( digitalRead(_ioPin) ){
            _eq.putQ(Msg::BUTTON_UP);
            _tick.detach();
            filterVal = 20;
            _state = State::BTN_IS_UP;
        }
        break;
    case State::BTN_IS_UP:
        if( !digitalRead(_ioPin) ){
            _eq.putQ(Msg::BUTTON_DOWN);
            _tick.attach(1.0, std::bind(&Buttons::timerCallBack, this));
            _btnPressedTime = 0;
            filterVal = 20;
            _state = State::BTN_IS_DOWN;
        }
        break;
    }
    return filterVal;
}

void Buttons::init()
{
    if( !digitalRead(_ioPin)){
        _state = State::BTN_IS_DOWN;
        _eq.putQ(Msg::BUTTON_DOWN);
        _tick.attach(1.0, std::bind(&Buttons::timerCallBack, this));
    }else
        _state = State::BTN_IS_UP;
}
