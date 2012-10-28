#include "widget.h"
#include "ui_widget.h"
#include "serial-port/robotcontrol.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->comboBox->addItems(RobotControl::GetPortNames());
    rc = new RobotControl(ui->comboBox->currentText(), ui->spinBox->value(), 500);
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

void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    rc = new RobotControl(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}

void Widget::on_spinBox_valueChanged(int arg1)
{
    rc = new RobotControl(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}
