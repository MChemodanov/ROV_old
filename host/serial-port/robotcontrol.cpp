#include "robotcontrol.h"

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

RobotControl::RobotControl(int engines)
{
    this->engines = engines;

    foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts())
    {
        // Check name or something
        serial.setPort(info);
    }

    timer = new QTimer();

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(TimerTick()));

    newSpeed = new int[engines];
    actualSpeed = new int[engines];
    newReverse = new int[engines];
    actualReverse = new int[engines];
    ticksSinceLastReverse = new int[engines];
    for(int i = 0; i < engines; i++)
    {
        newSpeed[i] = 0;
        actualSpeed[i] = 0;
        newReverse[i] = 0;
        actualReverse[i] = 0;
        ticksSinceLastReverse[i] = 0;
    }

    if(serial.open(QIODevice::ReadWrite))
        timer->start(500);
}

RobotControl::~RobotControl()
{
    delete newSpeed;
    delete actualSpeed;
    delete newReverse;
    delete actualReverse;
    delete timer;
    serial.close();
}

void RobotControl::SetSpeed(int speed, int engine)
{
    if (speed >= 0 && speed <= 255 && engine >= 0 && engine < engines)
        newSpeed[engine] = speed;
}

void RobotControl::SetReverse(int reverse, int engine)
{
    if (engine >= 0 && engine < engines && reverse >= 0 && reverse <= 1)
        newReverse[engine] = reverse;
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
        ticksSinceLastReverse[i]++;

    for (int i = 0; i < engines; i++)
    {
        if (newSpeed[i] != actualSpeed[i])
        {
            actualSpeed[i] = newSpeed[i];
            WriteSpeed(actualSpeed[i], i);
        }
        if (ticksSinceLastReverse[i] >= 4 && newReverse[i] != actualReverse[i])
        {
            ticksSinceLastReverse[i] = 0;
            actualReverse[i] = newReverse[i];
            WriteReverse(actualReverse[i], i);
        }
    }
}

void RobotControl::StartWriting()
{
    timer->start(500);
}

void RobotControl::StopWriting()
{
    timer->stop();
}

