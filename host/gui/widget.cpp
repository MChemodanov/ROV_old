#include "widget.h"
#include "ui_widget.h"
#include "serial-port/robotcontrol.h"

#include <QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    joystickEnabled(false),
    initialized(false)
{
    ui->setupUi(this);

    ui->comboBox->addItems(RobotControl::GetPortNames());
    ui->comboBox_2->addItems(JoystickControl::GetJoystickNames());

    if(RobotControl::GetPortNames().length() < 1)
        QMessageBox::warning(this, "Error", "No COM ports available!");
    else
    {
        if(rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 250) < 0)
            QMessageBox::warning(this, "Error", "Couldn't open COM port!");
    else
            initialized = true;
    }

    connect(&jc, SIGNAL(axisEvent(int,int,int)), this, SLOT(joystick_axisChanged(int,int,int)));
    connect(&jc, SIGNAL(buttonEvent(int,bool)), this, SLOT(joystick_buttonPressed(int,bool)));

    if(JoystickControl::GetJoystickNames().length() < 1)
        QMessageBox::warning(this, "Error", "No joysticks available!");
    else
        if(jc.Initialize(ui->comboBox_2->currentText(), 50) < 0)
            QMessageBox::warning(this, "Error", "Couldn't open joystick!");
    connect(&timer, SIGNAL(timeout()), this, SLOT(timer_tick()));
    timer.start(50);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::joystick_axisChanged(int arg1, int arg2, int arg3)
{
    if(joystickEnabled)
    {
        int x = arg1 * 255 / 32768,
            y =  - arg2 * 255 / 32768,
            z = arg3 * 255 / 32768;
        ui->verticalSlider_2->setValue(y);
        ui->verticalSlider->setValue(z);
        ui->horizontalSlider->setValue(x);
    }
}

void Widget::joystick_buttonPressed(int buttonId, bool state)
{
    switch(buttonId)
    {
        case 0:
        {
            rc.SetHalt(state);
            ui->checkBox_3->setChecked(state);
            break;
        }
        case 1:
        {
            if(state)
            {
                if(rc.EnginesStarted())
                {
                    rc.StopEngines();
                    ui->checkBox_2->setChecked(true);
                }
                else
                {
                    rc.StartEngines();
                    ui->checkBox_2->setChecked(false);
                }
            }
            break;
        }
        default:
        {
            qDebug() << "ID: " << buttonId << " State: " << state;
        }
    }
}

void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(initialized)
        rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}

void Widget::on_spinBox_valueChanged(int arg1)
{
    if(initialized)
        rc.Initialize(ui->comboBox->currentText(), ui->spinBox->value(), 500);
}

void Widget::on_checkBox_stateChanged(int arg1)
{
    joystickEnabled = arg1;
    ui->verticalSlider->setEnabled(!arg1);
    ui->verticalSlider_2->setEnabled(!arg1);
    ui->horizontalSlider->setEnabled(!arg1);
    ui->checkBox_2->setEnabled(!arg1);
    ui->checkBox_3->setEnabled(!arg1);
}

void Widget::on_verticalSlider_2_valueChanged(int value)
{
    rc.SetMoveSpeed(value);
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    rc.SetRotateSpeed(value);
}

void Widget::on_verticalSlider_valueChanged(int value)
{
    rc.SetVerticalSpeed(value);
}

void Widget::timer_tick()
{
    ui->label_7->setText("Vertical: " + QString(rc.GetReverse(0)?"R":"") + QString::number(rc.GetSpeed(0)));
    ui->label_5->setText("Right: " + QString(rc.GetReverse(1)?"R":"") + QString::number(rc.GetSpeed(1)));
    ui->label_6->setText("Left: " + QString(rc.GetReverse(2)?"R":"") + QString::number(rc.GetSpeed(2)));
}

void Widget::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(! jc.Initialize(arg1, 100) > 0)
        QMessageBox::warning(this, "Error", "Couldn't open joystick!");
}

void Widget::on_checkBox_2_toggled(bool checked)
{
    if(checked)
        rc.StopEngines();
    else
        rc.StartEngines();
}

void Widget::on_checkBox_3_toggled(bool checked)
{
    rc.SetHalt(checked);
}
