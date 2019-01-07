#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include<map>
#include <vector>

#include <Ticker.h>

#include "commondefs.h"
#include "msg.h"

enum class Ui{
    Main,
    Wifi,
    Menu
};


struct TimeElapsed{
const int secInDay{86400};
const int secInHour{3600};
    TimeElapsed(const uint32_t& t):
    days(t/secInDay),
    hours((t%secInDay)/secInHour),
    mins(((t%secInDay)%secInHour)/60),
    all(t){;}
    const uint32_t days;
    const uint32_t hours;
    const uint32_t mins;
    const uint32_t all;

};

class DisplayIf{
public:
    typedef std::vector<const char*> Menu;
    virtual void menuSelection(const char* title,const Menu& m,const uint8_t select=0)=0;
    virtual void YesNoQuestion(const char* title,const char* q,const boolean yes)=0;
    virtual void show(const char* title, const char* str)=0;
    virtual void showSOC(const CurrentData& cd,const uint8_t soc)=0;
    virtual void showStats(const BatteryStats& st,const TimeElapsed& te)=0;
};

class UserIf{
public:
    UserIf(const Ui& me,AppCtx* appCtx,DisplayIf* disp ):_me(me),_appCtx(appCtx),_disp(disp){;}

    virtual Ui next(){
        return _me;
    }

    virtual Ui selectAndAction(){
        return _me;
    }

    virtual Ui update(const Msg& msg)=0;
    virtual void before(){;}
    virtual void after(){;}

const Ui    _me;

protected:

    AppCtx*     _appCtx{nullptr};
    DisplayIf*  _disp{nullptr};

#ifdef DEBUG
    const char* _stName{nullptr};
#endif
};


class Main:public UserIf{

public:
    Main(const Ui& me,AppCtx* appCtx,DisplayIf *disp):UserIf(me,appCtx,disp)
    {
        ;
    }

    virtual Ui selectAndAction();
    virtual Ui next();
    virtual Ui update(const Msg& msg);
    virtual void before();
    virtual void after();
private:
    void timerCallBack();

    enum class St{
        Main,
        Stats
    };

    Ticker  _tick;
    St _st{St::Main};
};

class Wifi:public UserIf{

public:
    Wifi(const Ui& me,AppCtx* appCtx,DisplayIf *disp):UserIf(me,appCtx,disp){;}

    virtual Ui next();
    virtual Ui selectAndAction();
    virtual Ui update(const Msg& msg);

private:
    void timerCallBack();
    virtual void after();
    enum class St{
        Main,
        Selected,
        Connecting
    };

    Ticker  _tick;
    St _st{St::Main};
    DisplayIf::Menu _menu{"Connect Vessel monitor"};

};


class Menu:public UserIf{

public:
    Menu(const Ui& me,AppCtx* appCtx,DisplayIf *disp):UserIf(me,appCtx,disp){;}
    virtual Ui next();
    virtual Ui selectAndAction();
    virtual Ui update(const Msg& msg);
private:
    enum class St{
        Main,
        LoadMqttSelected,
        WaitMqttConf,
        LoadBattSelected,
        WaitBattConf,
        ResetDevSelected,
        Reset_Yes,
        Reset_No,
        Done

    };

    St _st{St::Main};
    DisplayIf::Menu _menu{"Load Mqtt Config","Load battery conf","Reset device"};

};


class MenuCtrl{

#define ADD_MENU(x,y) {x,new y(x,_appCtx,_disp)}
typedef std::map<Ui, UserIf*> UiMap;

public:
    MenuCtrl(AppCtx* appCtx,DisplayIf* disp ):
        _appCtx(appCtx),
        _disp(disp),
        _uis{ ADD_MENU(Ui::Main,Main),
              ADD_MENU(Ui::Menu,Menu),
              ADD_MENU(Ui::Wifi,Wifi)
            },
        _ui{_uis[Ui::Main]}{;}

    void handleMsg(const Msg& msg);

private:
    void selectMenu(const Ui& id);
    AppCtx*    _appCtx;
    DisplayIf* _disp;
    UiMap      _uis;
    UserIf*    _ui;

};

#endif
