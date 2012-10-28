QT += core gui

TARGET = RobotControlGUI
TEMPLATE = app

INCLUDEPATH += ..\
../serial-port

SOURCEPATH += ../serial-port

CONFIG += serialport

SOURCES += *.cpp \
    ../serial-port/robotcontrol.cpp
SOURCES +=

HEADERS += *.h \
    ../serial-port/robotcontrol.h

FORMS += widget.ui
