#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QTimer>
#include <QVector>
#include <QStringList>
#include <QtNetwork/QTcpSocket>

#include "auto_ptr.h"

#include "verticalcontroller.h"

class RobotControl : public QObject
{
    Q_OBJECT

public:
    explicit RobotControl(QObject *parent = 0);

    ~RobotControl();

    int Initialize(QString address, int port, int _engines, int _tickTime);

    void SetVerticalSpeed(int value);

    void SetMoveSpeed(int value);

    void SetRotateSpeed(int value);

    void SetPitchSpeed(int value);

    void SetHalt(bool state);

    void SetPitching(bool state);

    void StartEngines();

    void StopEngines();

    void StopWriting();

    void StartWriting();

    int GetSpeed(int engine);

    bool GetReverse(int engine);

    bool GetHalt();

    double GetDepth();

    double GetPitch();

    void SetTargetDepth(double tgDepth);

    void SetTargetPitch(double tgPitch);

    void SetManualControl(bool state);

    void SetPitchReg(bool state);

    void SetDepthReg(bool state);

    bool EnginesStarted();

    void OpenManip(int commands);

    void CloseManip(int commands);

protected:
    std::auto_ptr<VerticalController> vertc;
    QTcpSocket socket;
    QTimer timer;
    int engines, tickTime,
        ticksForReverse,
        lastEngine;
    int vertSpeed,
        moveSpeed,
        rotateSpeed,
        pitchSpeed;
    bool initialized, manualControl;

    struct EngineData
    {
        QVector<int> newSpeed,
                actualSpeed,
                newReverse,
                actualReverse,
                ticksSinceLastReverse;
        QVector<double> angle,
                        coeff;
    } ed;

    enum State
    {
        Stop,
        Move,
        Yaw,
        Pitch,
        Roll
    } currentState;

    virtual void CalcEnginesData();

    void WriteSpeed(int speed, int engine);

    void WriteReverse(int reverse, int engine);

    void SetSpeed(int speed, int engine);

    void SetReverse(int reverse, int engine);

private slots:
    void TimerTick();

    void SocketDisconnected();

signals:
    void Disconnected();
};

#endif // ROBOTCONTROL_H
