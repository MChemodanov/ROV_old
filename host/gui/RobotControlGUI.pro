QT += core gui

TARGET = RobotControlGUI
TEMPLATE = app

INCLUDEPATH += ..\

CONFIG += serialport

LIBS += -lSDL

SOURCES += *.cpp \
    ../serial-port/robotcontrol.cpp \
    ../joystick/joystickcontrol.cpp

HEADERS += *.h \
    ../serial-port/robotcontrol.h \
    ../joystick/joystickcontrol.h

FORMS += widget.ui
