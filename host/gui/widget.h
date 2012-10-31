#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "serial-port/robotcontrol.h"
#include "joystick/joystickcontrol.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_verticalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_verticalSlider_valueChanged(int value);

public slots:
    void joystick_axisChanged(int arg1, int arg2, int arg3);

private:
    Ui::Widget *ui;
    RobotControl *rc;
    JoystickControl *jc;
    bool rRev, lRev, vRev;
};

#endif // WIDGET_H
