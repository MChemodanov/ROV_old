#include "verticalcontroller.h"

VerticalController::VerticalController(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    if(socket->isOpen())
        this->socket = socket;
    depthc.reset(new DepthController(socket, this));
    diffc.reset(new DiffController(socket, this));
}

void VerticalController::CalcData()
{
    realDepth = depthc->GetDepth();
    realPitch = diffc->GetPitch();

    fwSpeed = depthc->GetVertSpeed() * diffc->GetForwardSpeed() * 255;
    bwSpeed = depthc->GetVertSpeed() * diffc->GetBackwardSpeed() * 255;

    if (qAbs(fwSpeed) > 255 || qAbs(bwSpeed) > 255)
    {
        double multiplier = qMax(qAbs(fwSpeed), qAbs(bwSpeed)) / 255.0;

        fwSpeed /= multiplier;
        bwSpeed /= multiplier;
    }
}

void VerticalController::SetTargetDepth(double depth)
{
    targetDepth = depth;
    depthc->SetTargetDepth(depth);
}

void VerticalController::SetTargetPitch(double pitch)
{
    targetPitch = pitch;
    diffc->SetTargetPitch(pitch);
}

int VerticalController::GetForwardSpeed()
{
    CalcData();
    return fwSpeed;
}

int VerticalController::GetBackwardSpeed()
{
    CalcData();
    return bwSpeed;
}

double VerticalController::GetDepth()
{
    return realDepth;
}

double VerticalController::GetPitch()
{
    return realPitch;
}
