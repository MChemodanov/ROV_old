#include "robotcontrol.h"
#include <QStringList>
#include <qtconcurrentexception.h>

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

RobotControl::RobotControl(QObject *parent) :
    QObject(parent)
{

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
    if(flag < 0)
        return flag;

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTick()));

    ed.newSpeed.resize(engines);
    ed.actualSpeed.resize(engines);
    ed.newReverse.resize(engines);
    ed.actualReverse.resize(engines);
    ed.ticksSinceLastReverse.resize(engines);

    if(serial.open(QIODevice::ReadWrite))
        timer.start(tickTime);
    else
        return -1;
    return 1;
}

void RobotControl::SetSpeed(int speed, int engine)
{
    if (speed >= 0 && speed <= 255 && engine >= 0 && engine < engines)
        ed.newSpeed[engine] = speed;
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
    for(int i = 0; i < engines; i++)
        ed.ticksSinceLastReverse[i]++;

    for (int i = 0; i < engines; i++)
    {
        if (ed.newSpeed[i] != ed.actualSpeed[i])
        {
            ed.actualSpeed[i] = ed.newSpeed[i];
            WriteSpeed(ed.actualSpeed[i], i);
        }
        if (ed.ticksSinceLastReverse[i] >= ticksForReverse &&
                ed.newReverse[i] != ed.actualReverse[i])
        {
            ed.ticksSinceLastReverse[i] = 0;
            ed.actualReverse[i] = ed.newReverse[i];
            WriteReverse(ed.actualReverse[i], i);
        }
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

