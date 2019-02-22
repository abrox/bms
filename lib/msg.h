#ifndef MSG_H
#define MSG_H

enum class Msg{
    DISPLAY_OFF_TIMEOUT,
    UPDATE_STATUS_DISLAY,
    BUTTON_DOWN,
    BUTTON_SHORT_PRESS,
    BUTTON_DOWN_TWO_SEC,
    BUTTON_DOWN_LONG,
    BUTTON_DOWN_EXTLONG,
    BUTTON_UP,
    MEASSURE_TIMEOUT,
    START_MQTT,
    MQTT_CLIENT_TIMEOUT,
    MQTT_RUNNING,
    MQTT_FAILED,
    SOC_UPDATE,
    CURRENT_REQ,
    MENU_RETURN_TIMEOUT
};

struct CurrentData{
     float volt{0.0};    ///< meassured voltage.
     float cu{0.0};      ///<  meassured currect in(+) or out(-).
     int16_t temp{INT16_MAX};  ///<Battery temperature F in deg.
     CurrentData(){}
     CurrentData(const float &v,const float &c,int16_t const &t):
         volt(v),cu(c),temp(t){}
};

struct BatteryStats{
    uint8_t soc{0};
    uint8_t soh{0};
};

#endif
