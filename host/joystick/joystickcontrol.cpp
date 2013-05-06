#include "joystickcontrol.h"
#include <QStringList>

JoystickControl::JoystickControl(QObject *parent) :
    QObject(parent),
    oldX(0),
    oldY(0),
    oldZ(0)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTick()));
}

JoystickControl::~JoystickControl()
{
}

int JoystickControl::Initialize(QString joystickName, int updateTime)
{
    int flag = -1;
    if(SDL_Init(SDL_INIT_JOYSTICK) >= 0)
    {
        for(int i = 0; i < SDL_NumJoysticks(); i++ )
            if (SDL_JoystickName(i) == joystickName)
            {
                joystick = SDL_JoystickOpen(i);
                timer.start(updateTime);
                flag = 1;
            }
        buttons = SDL_JoystickNumButtons(joystick);
        buttonStates.clear();
        buttonStates.resize(buttons);
        for(int i = 0; i < buttons; i++)
            buttonStates[i] = false;

        hats = SDL_JoystickNumHats(joystick);
        hatStates.clear();
        hatStates.resize(hats);
        for(int i = 0; i < hats; i++)
            hatStates[i] = Middle;
    }
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
    for(int i = 0; i < buttons; i++)
        if(SDL_JoystickGetButton(joystick, i) != buttonStates[i])
        {
            buttonStates[i] = SDL_JoystickGetButton(joystick, i);
            emit buttonEvent(i, buttonStates[i]);
        }
    for(int i = 0; i < hats; i++)
        if(SDL_JoystickGetHat(joystick, i) != hatStates[i])
        {
            hatStates[i] = SDL_JoystickGetHat(joystick, i);
            emit hatEvent(i, hatStates[i]);
        }
}

int JoystickControl::GetHatState(int hatId)
{
    return hatStates[hatId];
}
