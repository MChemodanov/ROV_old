#include "widget.h"
#include "ui_widget.h"
#include "serial-port/robotcontrol.h"

#include <QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    rRev(false),
    lRev(false),
    vRev(false)
{
    ui->setupUi(this);

    ui->comboBox->addItems(RobotControl::GetPortNames());
    ui->comboBox_2->addItems(JoystickControl::GetJoystickNames());

    //rc = new RobotControl(this);
    if(RobotControl::GetPortNames().length() < 1)
        QMessageBox::warning(this, "Error", "No COM ports available!");
    else
        if(rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 500) < 0)
            QMessageBox::warning(this, "Error", "Couldn't open COM port!");

    //jc = new JoystickControl(this);
    connect(&jc, SIGNAL(axisEvent(int,int,int)), this, SLOT(joystick_axisChanged(int,int,int)));

    if(JoystickControl::GetJoystickNames().length() < 1)
        QMessageBox::warning(this, "Error", "No joysticks available!");
    else
        if(jc.Initialize(ui->comboBox_2->currentText(), 100) < 0)
            QMessageBox::warning(this, "Error", "Couldn't open joystick!");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::joystick_axisChanged(int arg1, int arg2, int arg3)
{
    int x = arg1 * 255 / 32768,
        y =  - arg2 * 255 / 32768,
        z = arg3 * 255 / 32768;
    ui->verticalSlider_2->setValue(y);
    ui->verticalSlider->setValue(z);
    ui->horizontalSlider->setValue(x);
}

void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}

void Widget::on_spinBox_valueChanged(int arg1)
{
    rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}

void Widget::on_checkBox_stateChanged(int arg1)
{
    ui->verticalSlider->setEnabled(!arg1);
    ui->verticalSlider_2->setEnabled(!arg1);
    ui->horizontalSlider->setEnabled(!arg1);
}

void Widget::on_verticalSlider_2_valueChanged(int value)
{
    int rSpeed = value, lSpeed = value;
    if(ui->horizontalSlider->value() != 0)
    {
        double coeff = ui->horizontalSlider->value() * 1.0 / ui->horizontalSlider->maximum();
        if(ui->horizontalSlider->value() > 0)
        {
            rSpeed = qRound(rSpeed * (1.0 - coeff));
        }
        else
        {
            lSpeed = qRound(lSpeed * (1.0 - qAbs(coeff)));
        }
    }
    if (value != 0)
        if(value > 0)
        {
            rc.SetSpeed(rSpeed, 1);
            rc.SetReverse(0, 1);
            rRev = false;
            rc.SetSpeed(lSpeed, 2);
            rc.SetReverse(0, 2);
            lRev = false;
        }
        else
        {
            rc.SetSpeed(qAbs(rSpeed), 1);
            rc.SetReverse(1, 1);
            rRev = true;
            rc.SetSpeed(qAbs(lSpeed), 2);
            rc.SetReverse(1, 2);
            lRev = true;
        }

    else
    {
        if(value > 0)
        {
            rc.SetSpeed(ui->horizontalSlider->value(), 1);
            rc.SetReverse(1, 1);
            rRev = true;
            rc.SetSpeed(ui->horizontalSlider->value(), 2);
            rc.SetReverse(0, 2);
            lRev = false;
            lSpeed = rSpeed = ui->horizontalSlider->value();
        }
        else
        {
            rc.SetSpeed(ui->horizontalSlider->value(), 1);
            rc.SetReverse(0, 1);
            rRev = false;
            rc.SetSpeed(ui->horizontalSlider->value(), 2);
            rc.SetReverse(1, 2);
            lRev = true;
            lSpeed = rSpeed = ui->horizontalSlider->value();
        }
    }
    ui->label_5->setText("Right: " + QString(rRev?"R":"") + QString::number(qAbs(rSpeed)));
    ui->label_6->setText("Left: " + QString(lRev?"R":"") + QString::number(qAbs(lSpeed)));
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    int rSpeed = ui->verticalSlider_2->value(),
            lSpeed = ui->verticalSlider_2->value();
    if(value != 0)
    {
        double coeff = value * 1.0 / ui->horizontalSlider->maximum();
        if(value > 0)
        {
            rSpeed = qRound(rSpeed * (1.0 - coeff));
        }
        else
        {
            lSpeed = qRound(lSpeed * (1.0 - qAbs(coeff)));
        }
    }
    if(ui->verticalSlider_2->value() != 0)
        if(ui->verticalSlider_2->value() > 0)
        {
            rc.SetSpeed(rSpeed, 1);
            rc.SetReverse(0, 1);
            rRev = false;
            rc.SetSpeed(lSpeed, 2);
            rc.SetReverse(0, 2);
            lRev = false;
        }
        else
        {
            rc.SetSpeed(qAbs(rSpeed), 1);
            rc.SetReverse(1, 1);
            rRev = true;
            rc.SetSpeed(qAbs(lSpeed), 2);
            rc.SetReverse(1, 2);
            lRev = true;
        }
    else
    {
        if(value > 0)
        {
            rc.SetSpeed(value, 1);
            rc.SetReverse(1, 1);
            rRev = true;
            rc.SetSpeed(value, 2);
            rc.SetReverse(0, 2);
            lRev = false;
            lSpeed = rSpeed = value;
        }
        else
        {
            rc.SetSpeed(value, 1);
            rc.SetReverse(0, 1);
            rRev = false;
            rc.SetSpeed(value, 2);
            rc.SetReverse(1, 2);
            lRev = true;
            lSpeed = rSpeed = value;
        }
    }
    ui->label_5->setText("Right: " + QString(rRev?"R":"") + QString::number(qAbs(rSpeed)));
    ui->label_6->setText("Left: " + QString(lRev?"R":"") + QString::number(qAbs(lSpeed)));
}

void Widget::on_verticalSlider_valueChanged(int value)
{
    if(value >= 0)
    {
        rc.SetSpeed(value, 0);
        rc.SetReverse(0, 0);
        vRev = false;
    }
    else
    {
        rc.SetSpeed(qAbs(value), 0);
        rc.SetReverse(1, 0);
        vRev = true;
    }
    ui->label_7->setText("Vertical: " + QString(vRev?"R":"") + QString::number(qAbs(value)));
}

void Widget::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(! jc.Initialize(arg1, 100) > 0)
        QMessageBox::warning(this, "Error", "Couldn't open joystick!");
}
