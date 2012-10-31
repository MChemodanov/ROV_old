#include "joystickcontrol.h"
#include <QStringList>

JoystickControl::JoystickControl(QObject *parent) :
    QObject(parent),
    oldX(0),
    oldY(0),
    oldZ(0)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTick()));
}

JoystickControl::~JoystickControl()
{
    delete timer;
}

int JoystickControl::Initialize(QString joystickName, int updateTime)
{
    int flag = -1;
    if(SDL_Init(SDL_INIT_JOYSTICK) >= 0)
    {
        for(int i=0; i < SDL_NumJoysticks(); i++ )
            if (SDL_JoystickName(i) == joystickName)
            {
                joystick = SDL_JoystickOpen(i);
                flag = 1;
            }
    }
    timer->start(updateTime);
    return flag;
}

QStringList JoystickControl::GetJoystickNames()
{
    SDL_Init(SDL_INIT_JOYSTICK);
    QStringList list;
    for(int i=0; i < SDL_NumJoysticks(); i++ )
        list.append(SDL_JoystickName(i));
    SDL_Quit();
    return list;
}

void JoystickControl::TimerTick()
{
    SDL_JoystickUpdate();
    int newX = SDL_JoystickGetAxis(joystick, 0),
            newY = SDL_JoystickGetAxis(joystick, 1),
            newZ = SDL_JoystickGetAxis(joystick, 2);
    if(newX != oldX || newY != oldY || newZ != oldZ)
    {
        oldX = newX;
        oldY = newY;
        oldZ = newZ;
        emit axisEvent(newX, newY, newZ);
    }
}
