#include "depthcontroller.h"
#include <QDebug>

DepthController::DepthController(QTcpSocket *socket, QObject *parent) :
    QObject(parent),
    targetDepth(0)
{
    if(socket->isOpen())
        _socket = socket;
}

void DepthController::QueryDepth()
{
    if(!_socket->isWritable())
        return;
    _socket->write("#qd!");
    QString string;
    if(_socket->waitForReadyRead(2000))
        string = _socket->readLine();
    //else emit error
    int begin = string.indexOf("$qd");
    if (begin == -1)
        return;
    begin += 3;
    int end = string.indexOf("!",begin);

    if (end == -1)
        return;
    depth = string.mid(begin, end-begin).toDouble();

    CalcSpeed();
}

void DepthController::CalcSpeed()
{
    vertSpeed = 0;
    double difference = qAbs(targetDepth - depth);
    if (difference > 0.10)
    {
        vertSpeed = difference;
        vertSpeed *= (targetDepth > depth)? -1 : 1;
        if(vertSpeed > 1)
            vertSpeed = 1;
        if(vertSpeed < -1)
            vertSpeed = -1;
    }
}

void DepthController::SetTargetDepth(double tgDepth)
{
    targetDepth = tgDepth;
}

double DepthController::GetDepth()
{
    return depth;
}

int DepthController::GetVertSpeed()
{
    return vertSpeed;
}
