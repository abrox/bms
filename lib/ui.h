#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include<map>

#include "commondefs.h"
#include "msg.h"

enum class Ui{
    Main,
    Wifi,
    Menu
};

class UserIf{
public:
    UserIf(const Ui& me ):_me(me){;}

    virtual Ui next(){
        return _me;
    }

    virtual Ui selectAndAction(){
        return _me;
    }

    virtual void update(const Msg& msg)=0;


public:
    const Ui _me;


};

class Main:public UserIf{

public:
    Main(const Ui& me):UserIf(me){;}

    virtual Ui next();
    virtual void update(const Msg& msg);

};

class Wifi:public UserIf{

public:
    Wifi(const Ui& me):UserIf(me){;}

    virtual Ui next();
    virtual Ui selectAndAction();
    virtual void update(const Msg& msg);

private:
    enum class St{
        Main,
        Selected,
        Connecting,
        Connected,
        Failed
    };

    St _st{St::Main};

};


class Menu:public UserIf{

public:
    Menu(const Ui& me):UserIf(me){;}
    virtual void update(const Msg& msg);

};

class MenuCtrl{

public:
    void selectAndAction(const Msg& msg){
         Ui id  = _ui->selectAndAction();

         if( id != _ui->_me )
             _ui = _uis[id];

         _ui->update(msg);

    }

    void next(const Msg& msg){
        Ui id = _ui->next();

        if( id != _ui->_me )
            _ui = _uis[id];

        _ui->update(msg);
    }

    void update(const Msg& msg){
        _ui->update(msg);
    }
private:
#define ADD_MENU(x,y) {x,new y(x)}
typedef std::map<Ui, UserIf*> UiMap;

    UiMap _uis{ADD_MENU(Ui::Main,Main),
               ADD_MENU(Ui::Menu,Menu),
               ADD_MENU(Ui::Wifi,Wifi)
              };

    UserIf* _ui{_uis[Ui::Main]};

};

#endif
