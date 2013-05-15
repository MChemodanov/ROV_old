#include "verticalcontroller.h"

VerticalController::VerticalController(QTcpSocket *socket, QObject *parent) :
    QObject(parent),
    pitchReg(true),
    depthReg(true)
{
    if(socket->isOpen())
        this->socket = socket;
    else
        return;
    depthc.reset(new DepthController(socket, this));
    diffc.reset(new DiffController(socket, this));
    connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTick()));
    timer.start(200);
}

void VerticalController::TimerTick()
{
    depthc->QueryDepth();
    diffc->QueryPitch();
    realDepth = depthc->GetDepth();
    realPitch = diffc->GetPitch();
}

void VerticalController::CalcData()
{
    realDepth = depthc->GetDepth();
    realPitch = diffc->GetPitch();

    fwSpeed = 255;
    bwSpeed = 255;
    if(depthReg)
    {
        fwSpeed *= depthc->GetVertSpeed();
        bwSpeed *= depthc->GetVertSpeed();
    }
    if(pitchReg)
    {
        fwSpeed *= diffc->GetForwardSpeed();
        bwSpeed *= diffc->GetBackwardSpeed();
    }

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

void VerticalController::SetDepthReg(bool state)
{
    depthReg = state;
}

void VerticalController::SetPitchReg(bool state)
{
    pitchReg = state;
}
