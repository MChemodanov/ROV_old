QT += core gui network

TARGET = RobotControlGUI
TEMPLATE = app

INCLUDEPATH += ..\

CONFIG += serialport

DEFINES += "_USE_MATH_DEFINES"

LIBS += -lSDL
#LIBS += libSerialPort

SOURCES += *.cpp \
    ../robot-control/robotcontrol.cpp \
    ../robot-control/sixenginesrc.cpp \
    ../joystick/joystickcontrol.cpp

HEADERS += *.h \
    ../robot-control/robotcontrol.h \
    ../robot-control/sixenginesrc.h \
    ../joystick/joystickcontrol.h

FORMS += widget.ui
