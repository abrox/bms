TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    testbattery.cpp

SOURCES += ../battery.cpp

HEADERS += ../battery.h

INCLUDEPATH+= /usr/local/include
LIBS += -L/usr/local/lib -lUnitTest++
