#include "robotcontrol.h"
#include <QStringList>
#include <qtconcurrentexception.h>

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

RobotControl::RobotControl(QObject *parent) :
    QObject(parent),
    vertSpeed(0),
    moveSpeed(0),
    rotateSpeed(0),
    halt(false),
    enginesStarted(true)
{
    lastEngine = 0;
}

RobotControl::~RobotControl()
{
    serial.close();
}

int RobotControl::Initialize(QString portName, int _engines, int _tickTime)
{
    engines = _engines;
    tickTime = _tickTime;
    ticksForReverse = qRound(2000.0/tickTime + 0.5); //ceil((2 seconds)/(one tick))

    int flag = -1;
    foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts())
        if(info.portName() == portName)
        {
            serial.setPort(info);
            flag = 1;
        }
    //if(flag < 0)
        //return flag;

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTick()));

    ed.newSpeed.resize(engines);
    ed.newSpeed.push_back(0);
    ed.actualSpeed.resize(engines);
    ed.newReverse.resize(engines);
    ed.actualReverse.resize(engines);
    ed.ticksSinceLastReverse.resize(engines);
    timer.start(tickTime);

    if(!serial.open(QIODevice::ReadWrite))
        return -1;
    return 1;
}

void RobotControl::SetVerticalSpeed(int value)
{
    vertSpeed = value;
    CalcEnginesData();
}

void RobotControl::SetMoveSpeed(int value)
{
    moveSpeed = value;
    CalcEnginesData();
}

void RobotControl::SetRotateSpeed(int value)
{
    rotateSpeed = value;
    CalcEnginesData();
}

void RobotControl::SetHalt(bool state)
{
    halt = state;
    CalcEnginesData();
}

void RobotControl::StartEngines()
{
    enginesStarted = true;
    CalcEnginesData();
}

void RobotControl::StopEngines()
{
    enginesStarted = false;
    CalcEnginesData();
}

void RobotControl::CalcEnginesData()
{
    SetSpeed(vertSpeed, 0);
    if (!enginesStarted)
        for(int i = 0; i < engines; i++)
            SetSpeed(0, i);
    else
    {
        if (halt)
        {
            SetSpeed(rotateSpeed, 1);
            SetSpeed(-rotateSpeed, 2);
        }
        else
        {
            if(rotateSpeed >= 0)
            {
                SetSpeed(moveSpeed - rotateSpeed, 1);
                SetSpeed(moveSpeed, 2);
            }
            else
            {
                SetSpeed(moveSpeed, 1);
                SetSpeed(moveSpeed + rotateSpeed, 2);
            }
        }
    }

}

void RobotControl::SetSpeed(int speed, int engine)
{
    if (engine >= 0 && engine < engines)
    {
        if(speed >= -255 && speed < 0)
        {
            ed.newSpeed[engine] = -speed;
            ed.newReverse[engine] = 1;
        }
        else
            if(speed >= 0 && speed <= 255)
            {
                ed.newSpeed[engine] = speed;
                ed.newReverse[engine] = 0;
            }
    }
}

void RobotControl::SetReverse(int reverse, int engine)
{
    if (engine >= 0 && engine < engines && reverse >= 0 && reverse <= 1)
        ed.newReverse[engine] = reverse;
}

void RobotControl::WriteSpeed(int speed, int engine)
{
    QString toWrite = "#p" + QString::number(engine) + QString::number(speed) + '!';
    if(serial.isWritable())
        serial.write(toWrite.toStdString().c_str());
}

void RobotControl::WriteReverse(int reverse, int engine)
{
    QString toWrite ="#r" + QString::number(engine) + QString::number(reverse) + '!';
    if(serial.isWritable() && engine >= 0 && reverse >= 0 && reverse <=1)
        serial.write(toWrite.toStdString().c_str());
}

void RobotControl::TimerTick()
{
    for(int i = lastEngine; i < engines; i++)
        ed.ticksSinceLastReverse[i]++;

    lastEngine++;

        if(lastEngine >= engines)
            lastEngine = 0;

        if (ed.ticksSinceLastReverse[lastEngine] >= ticksForReverse &&
                ed.newReverse[lastEngine] != ed.actualReverse[lastEngine])
        {
            ed.ticksSinceLastReverse[lastEngine] = 0;
            ed.actualReverse[lastEngine] = ed.newReverse[lastEngine];
            WriteReverse(ed.actualReverse[lastEngine], lastEngine);
        }else
        //if (ed.newSpeed[lastEngine] != ed.actualSpeed[lastEngine])
        {
            ed.actualSpeed[lastEngine] = ed.newSpeed[lastEngine];
            WriteSpeed(ed.actualSpeed[lastEngine], lastEngine);
        }
}

void RobotControl::StartWriting()
{
    timer.start(tickTime);
}

void RobotControl::StopWriting()
{
    timer.stop();
}

QStringList RobotControl::GetPortNames()
{
    QStringList list;
    foreach(const SerialPortInfo &info, SerialPortInfo::availablePorts())
        list.insert(0, info.portName());
    return list;
}

int RobotControl::GetSpeed(int engine)
{
    if(!ed.actualSpeed.isEmpty())
        return ed.actualSpeed[engine];
    else return 0;
}

bool RobotControl::GetReverse(int engine)
{
    if(!ed.actualReverse.isEmpty())
        return ed.actualReverse[engine];
    else return false;
}

bool RobotControl::GetHalt()
{
    return halt;
}

bool RobotControl::EnginesStarted()
{
    return enginesStarted;
}
