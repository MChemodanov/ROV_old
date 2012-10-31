#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QTimer>
#include <QVector>
#include <QStringList>

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

QT_USE_NAMESPACE_SERIALPORT

class RobotControl : QObject
{
    Q_OBJECT

public:
    explicit RobotControl(QObject *parent = 0);

    ~RobotControl();

    int Initialize(QString portName, int _engines, int _tickTime);

    void SetSpeed(int speed, int engine);

    void SetReverse(int reverse, int engine);

    void StopWriting();

    void StartWriting();

    static QStringList GetPortNames();

private:
    SerialPort serial;
    QTimer timer;
    int engines, tickTime,
        ticksForReverse;

    struct EngineData
    {
        QVector<int> newSpeed,
                actualSpeed,
                newReverse,
                actualReverse,
                ticksSinceLastReverse;
    } ed;

    void WriteSpeed(int speed, int engine);

    void WriteReverse(int reverse, int engine);

private slots:
    void TimerTick();
};

#endif // ROBOTCONTROL_H
