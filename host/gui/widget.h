#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "robotcontrol.h"

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
    void on_dial_valueChanged(int value);

    void on_checkBox_toggled(bool checked);

private:
    Ui::Widget *ui;
    RobotControl *rc;
};

#endif // WIDGET_H
