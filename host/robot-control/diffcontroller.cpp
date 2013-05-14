#include "diffcontroller.h"

DiffController::DiffController(QTcpSocket *socket, QObject *parent) :
    QObject(parent),
    targetPitch(0)
{
    if(socket->isOpen())
        _socket = socket;
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
    int begin = string.indexOf("$qp");
    if (begin == -1)
        return;
    begin += 3;
    int end = string.indexOf("!",begin);

    if (end == -1)
        return;

    pitch = string.mid(begin, end-begin).toDouble();

    CalcSpeeds();
}

void DiffController::CalcSpeeds()
{
    fwSpeed = bwSpeed = 1;
    double difference = qAbs(targetPitch - pitch);
    if(difference > 5)
    {
        double proportion = difference / 30;
        if(targetPitch > pitch)
        {

            bwSpeed = 1 + proportion;
            fwSpeed = 1 - proportion;
        }
        else
        {
            fwSpeed = 1 + proportion;
            bwSpeed = 1 - proportion;
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

double DiffController::GetBackwardSpeed()
{
    return bwSpeed;
}

double DiffController::GetForwardSpeed()
{
    return fwSpeed;
}
