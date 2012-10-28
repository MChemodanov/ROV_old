#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QTimer>

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

QT_USE_NAMESPACE_SERIALPORT

class RobotControl : QObject
{
    Q_OBJECT

public:
    RobotControl(int engines);

    ~RobotControl();

    void SetSpeed(int speed, int engine);

    void SetReverse(int reverse, int engine);

    void StopWriting();

    void StartWriting();

private:
    SerialPort serial;
    QTimer* timer;
    int engines;
    int *newSpeed, *actualSpeed;
    int *newReverse, *actualReverse;
    int *ticksSinceLastReverse;

    void WriteSpeed(int speed, int engine);

    void WriteReverse(int reverse, int engine);

private slots:
    void TimerTick();
};

#endif // ROBOTCONTROL_H
