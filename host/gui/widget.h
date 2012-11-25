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

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_checkBox_2_toggled(bool checked);

    void on_checkBox_3_toggled(bool checked);

public slots:
    void joystick_axisChanged(int arg1, int arg2, int arg3);

    void joystick_buttonPressed(int buttonId, bool state);

    void timer_tick();

private:
    QTimer timer;
    Ui::Widget *ui;
    RobotControl rc;
    JoystickControl jc;
    bool joystickEnabled,
            initialized;
};

#endif // WIDGET_H
