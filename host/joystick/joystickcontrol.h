#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <QObject>
#include <QTimer>
#include <SDL/SDL.h>

class JoystickControl : public QObject
{
    Q_OBJECT
public:
    explicit JoystickControl(QObject *parent = 0);

    ~JoystickControl();

    int Initialize(QString joystickName, int updateTime);

    static QStringList GetJoystickNames();

private:
    int oldX, oldY, oldZ;
    QTimer *timer;
    SDL_Joystick *joystick;

signals:
    void axisEvent(int arg1, int arg2, int arg3);
    
private slots:
    void TimerTick();
};

#endif // JOYSTICKCONTROL_H
