#include"ui.h"

Ui Main::next(){
    Ui next=Ui::Main;
    switch (_st) {
    case St::Main:
        CHANGE_STATE(St::Stats);
        break;
    case St::Stats:
        CHANGE_STATE(St::Main);
        break;
    default:
        break;
    }

    return next;
}

Ui Main::selectAndAction()
{
    Ui next=Ui::Main;
    switch (_st) {
    case St::Main:
        next = Ui::Wifi;
        break;
    case St::Stats:
        ;
        break;
    default:
        break;
    }

    return next;
}

Ui Main::update(const Msg& msg)
{
    Ui next=Ui::Main;

    if( msg == Msg::BUTTON_DOWN){
        _tick.detach();
    }else
     if( msg == Msg::BUTTON_UP ){
         _tick.attach(10, std::bind(&Main::timerCallBack, this));
    }

    switch (_st) {
    case St::Main:
        if( msg == Msg::SOC_UPDATE ){
            _disp->showSOC(_appCtx->_currentData,_appCtx->_batteryStats.soc);
        }
        break;
    case St::Stats:
        if( msg == Msg::SOC_UPDATE ){
            TimeElapsed te(rtc.now().unixtime()-_appCtx->_startTime);
            _disp->showStats(_appCtx->_batteryStats,te);
        }
        break;
    default:
        break;
    }
    return next;
}

void Main::before()
{
    _disp->show("Main","Wait current..");
}

void Main::after()
{
}
void Main::timerCallBack()
{
   _appCtx->_eq.putQ(Msg::DISPLAY_OFF_TIMEOUT);
}

//////////////////////////////////////////////////
Ui Wifi::next(){
    Ui next=Ui::Wifi;
    switch (_st) {
    case St::Main:
        CHANGE_STATE(St::Selected);
        break;
    case St::Selected:
        CHANGE_STATE(St::Main);
        break;
    case St::Connecting:
        CHANGE_STATE( St::Selected);
        break;
    default:
        break;
    }

    return next;
}

Ui Wifi::selectAndAction()
{
    Ui next=Ui::Wifi;
    switch (_st) {
    case St::Main:
        next=Ui::Menu;
        break;
    case St::Selected:
        CHANGE_STATE(St::Connecting);
        _appCtx->_eq.putQ(Msg::START_MQTT);
        break;
    default:
        break;
    }

    return next;
}

Ui Wifi::update(const Msg& msg)
{
    Ui next=Ui::Wifi;

    switch (_st) {
    case St::Main:
        _disp->menuSelection("Wifi",_menu,0);
        break;
    case St::Selected:
         _disp->menuSelection("Wifi",_menu,1);
        break;
    case St::Connecting:
        if( msg == Msg::UPDATE_STATUS_DISLAY ){
            _disp->show("Wifi",_appCtx->_statusMsg);
        }else
         if( msg == Msg::MQTT_FAILED ||
             msg == Msg::MQTT_RUNNING){
             _tick.once(5, std::bind(&Wifi::timerCallBack, this));
        }else
         if( msg == Msg::MENU_RETURN_TIMEOUT ){
             next = Ui::Main;
         }
        break;
    default:
        break;
    }
    return next;
}
void Wifi::timerCallBack()
{
   _appCtx->_eq.putQ(Msg::MENU_RETURN_TIMEOUT);
}
void Wifi::after()
{
    CHANGE_STATE(St::Main);
}
/////////////////////////////////////////////////
Ui Menu::update(const Msg& msg)
{
    Ui next=Ui::Menu;

    switch (_st) {
    case St::Main:
        _disp->menuSelection("Menu",_menu,0);
        break;
    case St::LoadMqttSelected:
        _disp->menuSelection("Menu",_menu,1);
        break;
    case St::WaitMqttConf:
        _disp->show("Menu","WaitMqttConf");
        break;
    case St::LoadBattSelected:
        _disp->menuSelection("Menu",_menu,2);
        break;
    case St::WaitBattConf:
        _disp->show("Menu","WaitBattConf");
        break;
    case St::ResetDevSelected:
        _disp->menuSelection("Menu",_menu,3);
        break;
    case St::Reset_Yes:
        _disp->YesNoQuestion("Menu","Reset Device ?",true);
        break;
    case St::Reset_No:
        _disp->YesNoQuestion("Menu","Reset Device ?",false);
        break;
    case St::Done:
        _disp->show("Menu","Done");
        break;
    default:
        break;
    }
    return next;
}
Ui Menu::next()
{
    Ui next=Ui::Menu;
    switch (_st) {
    case St::Main:
        CHANGE_STATE(St::LoadMqttSelected);
        break;
    case St::LoadMqttSelected:
        CHANGE_STATE(St::LoadBattSelected);
        break;
    case St::WaitMqttConf:
        CHANGE_STATE(St::LoadMqttSelected);
        break;
    case St::LoadBattSelected:
        CHANGE_STATE(St::ResetDevSelected);
        break;
    case St::WaitBattConf:
        CHANGE_STATE(St::LoadBattSelected);
        break;
    case St::ResetDevSelected:
        CHANGE_STATE(St::Main);
        break;
    case St::Reset_Yes:
        CHANGE_STATE(St::Reset_No);
        break;
    case St::Reset_No:
        CHANGE_STATE(St::Reset_Yes);
        break;
    case St::Done:
        break;
    default:
        break;
    }
    return next;
}

Ui Menu::selectAndAction()
{
    Ui next=Ui::Menu;
    switch (_st) {
    case St::Main:
        next=Ui::Main;
        break;
    case St::LoadMqttSelected:
        CHANGE_STATE(St::WaitMqttConf);
        break;
    case St::WaitMqttConf:
        break;
    case St::LoadBattSelected:
        CHANGE_STATE(St::WaitBattConf);
        break;
    case St::WaitBattConf:
        break;
    case St::ResetDevSelected:
        CHANGE_STATE(St::Reset_Yes);
        break;
    case St::Reset_Yes:
        CHANGE_STATE(St::Done);
        break;
    case St::Reset_No:
        CHANGE_STATE(St::Main);
        break;
    case St::Done:
        break;
    default:
        break;
    }
    return next;
}

/////////////////////////////////////////////////

void MenuCtrl::handleMsg(const Msg& msg)
{
    Ui id;
    if( msg == Msg::BUTTON_SHORT_PRESS ){
        id = _ui->next();
        selectMenu(id);
    }else
    if( msg == Msg::BUTTON_DOWN_TWO_SEC ){
        id = _ui->selectAndAction();
        selectMenu(id);
    }

    id = _ui->update(msg);
    selectMenu(id);
}

void MenuCtrl::selectMenu(const Ui& id)
{
    if( id != _ui->_me ){
        _ui->after();
        _ui = _uis[id];
        _ui->before();
    }
}
