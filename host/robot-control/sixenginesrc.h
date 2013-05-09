#ifndef SIXENGINESRC_H
#define SIXENGINESRC_H

#include "robotcontrol.h"

class SixEnginesRC : public RobotControl
{

public:
    int Initialize(QString address, int port, int _engines, int _tickTime);

private:
    void CalcEnginesData();

    void GetEngineCoeffs(int x, int y);
};

#endif // SIXENGINESRC_H
