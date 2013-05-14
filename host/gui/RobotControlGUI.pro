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
    ../robot-control/diffcontroller.cpp \
    ../robot-control/depthcontroller.cpp \
    ../robot-control/verticalcontroller.cpp \
    ../joystick/joystickcontrol.cpp

HEADERS += *.h \
    ../joystick/joystickcontrol.h \
    ../robot-control/robotcontrol.h \
    ../robot-control/sixenginesrc.h \
    ../robot-control/diffcontroller.h \
    ../robot-control/depthcontroller.h \
    ../robot-control/verticalcontroller.h

FORMS += widget.ui
