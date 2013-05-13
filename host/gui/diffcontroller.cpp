#include "diffcontroller.h"

DiffController::DiffController(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    if(socket->isOpen())
    {
        _socket = socket;
        connect(&timer, SIGNAL(timeout()), this, SLOT(timerTick()));
        timer.start(1000);
    }
}

void DiffController::timerTick()
{
    QueryPitch();
    CalcSpeeds();
}

void DiffController::QueryPitch()
{
    if(!_socket->isWritable())
        return;
    _socket->write("#qp!");
    QString string;
    if(_socket->waitForReadyRead(2000))
        string = _socket->readLine();
    //else emit error
    if(string != "" && string.startsWith("$qp") && string.endsWith("!"))
        pitch = string.mid(3, string.length() - 4).toDouble();
}

void DiffController::CalcSpeeds()
{
    fwSpeed = bwSpeed = 1;
    double difference = qAbs(targetPitch - pitch);
    if(difference > 5)
    {
        if(targetPitch < pitch)
        {
            bwSpeed = difference / 90;
            fwSpeed = 2 - bwSpeed;
        }
        else
        {
            fwSpeed = difference / 90;
            bwSpeed = 2 - fwSpeed;
        }
    }
}

void DiffController::SetTargetPitch(double tgPitch)
{
    targetPitch = tgPitch;
}

double DiffController::GetPitch()
{
    return pitch;
}

int DiffController::GetBackwardSpeed()
{
    return bwSpeed;
}

int DiffController::GetForwardSpeed()
{
    return fwSpeed;
}
