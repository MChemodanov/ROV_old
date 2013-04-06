QT += core gui network

TARGET = RobotControlGUI
TEMPLATE = app

INCLUDEPATH += ..\

CONFIG += serialport

DEFINES += "_USE_MATH_DEFINES"

LIBS += -lSDL
#LIBS += libSerialPort

SOURCES += *.cpp \
    ../serial-port/robotcontrol.cpp \
    ../serial-port/sixenginesrc.cpp \
    ../joystick/joystickcontrol.cpp

HEADERS += *.h \
    ../serial-port/robotcontrol.h \
    ../serial-port/sixenginesrc.h \
    ../joystick/joystickcontrol.h \
    enginesmap.h

FORMS += widget.ui
