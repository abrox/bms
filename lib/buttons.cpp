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

Buttons::Buttons(eQueue_t &eq,const uint8_t ioPin):Runnable(eq),_st(State::BTN_INIT),_ioPin(ioPin),_btnPressedTime(0),_settleTime(0)
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
    int val = digitalRead(_ioPin);
    if( val == _ioState ){
        if( _settleTime )
            _settleTime--;
    }else{
        _settleTime=10;
        _ioState = val;
    }

    if( !_settleTime){
        checkBtn();
    }
    
}

void Buttons::checkBtn()
{
    switch(_st){
    case State::BTN_INIT:
        if( _ioState == HIGH ){
            CHANGE_STATE(State::BTN_IS_UP);
        }
        break;
    case State::BTN_IS_DOWN:
        //Release button...
        if( _ioState == HIGH ){
            _eq.putQ(Msg::BUTTON_UP);
            if( _btnPressedTime<2 )
                _eq.putQ(Msg::BUTTON_SHORT_PRESS);
            _tick.detach();
            CHANGE_STATE(State::BTN_IS_UP);
        }
        break;
    case State::BTN_IS_UP:
        //Pressing button...
        if( _ioState == LOW ){
            _eq.putQ(Msg::BUTTON_DOWN);
            _tick.attach(1.0, std::bind(&Buttons::timerCallBack, this));
            _btnPressedTime = 0;
            CHANGE_STATE(State::BTN_IS_DOWN);
        }
        break;
    }
}

void Buttons::init()
{
    _ioState = digitalRead(_ioPin);
    if( _ioState == LOW ){
        //Info sandman to keep system wake.
        _eq.putQ(Msg::BUTTON_DOWN);
    }
}
