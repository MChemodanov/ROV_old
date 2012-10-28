#include "widget.h"
#include "ui_widget.h"
#include "serial-port/robotcontrol.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    rc = new RobotControl(1);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_dial_valueChanged(int value)
{
    rc->SetSpeed(value, 0);
}

void Widget::on_checkBox_toggled(bool checked)
{
    rc->SetReverse(checked?1:0, 0);
}
