#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QTimer>
#include <QVector>
#include <QStringList>
#include <QtNetwork/QTcpSocket>

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

    void SetVerticalSpeed(int value);

    void SetMoveSpeed(int value);

    void SetRotateSpeed(int value);

    void SetHalt(bool state);

    void StartEngines();

    void StopEngines();

    void StopWriting();

    void StartWriting();

    static QStringList GetPortNames();

    int GetSpeed(int engine);

    bool GetReverse(int engine);

    bool GetHalt();

    bool EnginesStarted();

private:
    //SerialPort serial;
    QTcpSocket socket;
    QTimer timer;
    int engines, tickTime,
        ticksForReverse,
        lastEngine;
    int vertSpeed,
        moveSpeed,
        rotateSpeed;
    bool halt,
            initialized,
            enginesStarted;

    struct EngineData
    {
        QVector<int> newSpeed,
                actualSpeed,
                newReverse,
                actualReverse,
                ticksSinceLastReverse;
    } ed;

    void CalcEnginesData();

    void WriteSpeed(int speed, int engine);

    void WriteReverse(int reverse, int engine);

    void SetSpeed(int speed, int engine);

    void SetReverse(int reverse, int engine);

private slots:
    void TimerTick();
};

#endif // ROBOTCONTROL_H
