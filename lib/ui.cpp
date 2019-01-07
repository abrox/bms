#include"ui.h"

Ui Main::next()
{
    return Ui::Wifi;
}

void Main::update(const Msg& msg)
{
    ;
}

//////////////////////////////////////////////////
Ui Wifi::next(){
    Ui next=Ui::Wifi;
    switch (_st) {
    case St::Main:
        _st = St::Selected;
        break;
    case St::Selected:
        _st = St::Main;
        break;
    case St::Connecting:
        _st = St::Selected;
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
        _st = St::Connecting;
        break;
    case St::Connecting:
        ;
        break;
    default:
        break;
    }

    return next;
}

void Wifi::update(const Msg& msg)
{
    switch (_st) {
    case St::Main:
        ;
        break;
    case St::Selected:
        ;
        break;
    case St::Connecting:
        ;
        break;
    default:
        break;
    }
}

/////////////////////////////////////////////////
void Menu::update(const Msg& msg)
{

}

/////////////////////////////////////////////////
