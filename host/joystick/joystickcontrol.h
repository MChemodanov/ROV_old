#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <SDL/SDL.h>

class JoystickControl : public QObject
{
    Q_OBJECT
public:
    explicit JoystickControl(QObject *parent = 0);

    ~JoystickControl();

    int Initialize(QString joystickName, int updateTime);

    static QStringList GetJoystickNames();

    int GetHatState(int hatId);

    static enum HatStates
    {
        Middle = 0x00,
        Up = 0x01,
        Right = 0x02,
        Down = 0x04,
        Left = 0x08,
        RightUp = Up|Right,
        RightDown = Down|Right,
        LeftUp = Up|Left,
        LeftDown = Down|Left
    } HatStates;

private:
    int buttons, hats,
        oldX, oldY, oldZ;
    QVector<bool> buttonStates;
    QVector<int> hatStates;
    QTimer timer;
    SDL_Joystick *joystick;

signals:
    void axisEvent(int arg1, int arg2, int arg3);

    void buttonEvent(int buttodId, bool state);

    void hatEvent(int hatId, int state);
    
private slots:
    void TimerTick();
};

#endif // JOYSTICKCONTROL_H
