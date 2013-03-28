#include "sixenginesrc.h"
#include "robotcontrol.h"
#include <qmath.h>
#include <math.h>

typedef RobotControl base;

int SixEnginesRC::Initialize(QString address, int port, int _engines, int _tickTime)
{
    int result = base::Initialize(address, port, _engines, _tickTime);
    ed.coeff.resize(_engines);
    ed.angle.resize(_engines);
    ed.angle[2] = M_PI_4 * 5;
    ed.angle[3] = M_PI_4 * 7;
    ed.angle[4] = M_PI_4 * 3;
    ed.angle[5] = M_PI_4;
    return result;
}

void SixEnginesRC::CalcEnginesData()
{
    // 2     4
    //    0
    //    1
    // 3     5
    switch(currentState)
    {
        case Move:
        {
            SetSpeed(vertSpeed, 0);
            SetSpeed(vertSpeed, 1);
            GetEngineCoeffs(moveSpeed, rotateSpeed);
            for (int i = 2; i < 6; i ++)
                SetSpeed(ed.coeff[i] * (qAbs(moveSpeed) > qAbs(rotateSpeed)? moveSpeed : rotateSpeed), i);
            break;
        }
        case Stop:
        {
            for(int i = 0; i < engines; i++)
                SetSpeed(0, i);
            break;
        }
        case Yaw:
        {
            SetSpeed(vertSpeed, 0);
            SetSpeed(vertSpeed, 1);
            if(rotateSpeed >= 0)
            {
                SetSpeed(-rotateSpeed, 2);
                SetSpeed(rotateSpeed, 3);
                SetSpeed(rotateSpeed, 4);
                SetSpeed(-rotateSpeed, 5);
            }
            else
            {
                SetSpeed(rotateSpeed, 2);
                SetSpeed(-rotateSpeed, 3);
                SetSpeed(-rotateSpeed, 4);
                SetSpeed(rotateSpeed, 5);
            }
            break;
        }
        case Pitch:
        {
            for(int i = 2; i < engines; i++)
                SetSpeed(0, i);
            if (moveSpeed >= 0)
            {
                SetSpeed(-moveSpeed, 0);
                SetSpeed(moveSpeed, 1);
            }
            else
            {
                SetSpeed(moveSpeed, 0);
                SetSpeed(-moveSpeed, 1);
            }
            break;
        }
    }
}

void SixEnginesRC::GetEngineCoeffs(int x, int y)
{
    double maxCoeff = 0;
    for (int i = 2; i < 6; i++)
    {
    //Angle between engine direction and movement vector direction
    double alpha = qAbs(ed.angle[i] - qAtan(y * 1.0 / x));

    ed.coeff[i] = qCos(alpha);

    if(qAbs(ed.coeff[i]) > maxCoeff)
        maxCoeff = qAbs(ed.coeff[i]);
    }
    double multiplier = 1 / maxCoeff;
    for (int i = 2; i < 6; i++)
        ed.coeff[i] *= multiplier;
}
