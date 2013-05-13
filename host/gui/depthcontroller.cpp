#include "depthcontroller.h"
#include <QDebug>

DepthController::DepthController(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    if(socket->isOpen())
    {
        _socket = socket;
        connect(&timer, SIGNAL(timeout()), this, SLOT(timerTick()));
        timer.start(1000);
    }
}

void DepthController::timerTick()
{
    QueryDepth();
    CalcSpeed();
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
    if(string != "" && string.startsWith("$qd") && string.endsWith("!"))
        depth = string.mid(3, string.length() - 4).toDouble();
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
