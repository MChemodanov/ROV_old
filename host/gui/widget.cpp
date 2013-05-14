#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    joystickEnabled(false),
    initialized(false)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    ui->setupUi(this);
    LoadConfig("rc.cfg");

    em.SetBackgroundImage(QImage("ROV.png"));
    em.LoadConfig("em.cfg");
    ui->gridLayout->addWidget(&em, 0, 0);
    ui->gridLayoutWidget->setLayout(ui->gridLayout);

    ui->joystickCombo->addItems(JoystickControl::GetJoystickNames());

    connect(&jc, SIGNAL(axisEvent(int,int,int)), this, SLOT(joystick_axisChanged(int,int,int)));
    connect(&jc, SIGNAL(buttonEvent(int,bool)), this, SLOT(joystick_buttonPressed(int,bool)));
    connect(&rc, SIGNAL(Disconnected()), this, SLOT(RCDisconnected()));

    if(JoystickControl::GetJoystickNames().length() < 1)
        QMessageBox::warning(this, "Error", "No joysticks available!");
    else
        if(jc.Initialize(ui->joystickCombo->currentText(), 50) < 0)
            QMessageBox::warning(this, "Error", "Couldn't open joystick!");

    connect(&timer, SIGNAL(timeout()), this, SLOT(timer_tick()));
    timer.start(100);
}

Widget::~Widget()
{
    initialized = false;
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
        case 2:
        {
            rc.SetPitching(state);
            break;
        }
        default:
        {
            qDebug() << "ID: " << buttonId << " State: " << state;
        }
    }
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
    double depth = int(value / 25.5) + 10;
    rc.SetTargetDepth(depth);
    ui->heightLabel->setText("Высота");
    if(ui->autoHeightCheck->isChecked())
        ui->heightLabel->setText(QString(tr("Высота\n%1 м")).arg(depth));
}

void Widget::timer_tick()
{
    if (initialized)
    {
        for (int i = 0; i < 6; i++)
        {
            em.SetSpeed(i, rc.GetSpeed(i)/2.55);
            em.SetReverse(i, rc.GetReverse(i));
        }
        double depth = rc.GetDepth(), pitch = rc.GetPitch();
        ui->label_4->setText(QString("Depth: %1\nPitch: %2").arg(QString::number(depth)).arg(QString::number(pitch)));

        int hat = jc.GetHatState(0);
        if(hat == jc.Up)
            rc.OpenManip(1);
        if(hat == jc.Down)
            rc.CloseManip(1);
    }
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

void Widget::on_connectBtn_clicked()
{
    if(rc.Initialize(ui->ipEdit->text(), ui->portSpinBox->value(), ui->enginesSpinBox->value(), 100) < 0)
        QMessageBox::warning(this, "Error", "Couldn't open socket!");
    else
    {
        initialized = true;
        ui->connectBtn->setEnabled(false);
        ui->connectBtn->setText("Connected");
        ui->horizontalSlider->setEnabled(true);
        ui->verticalSlider->setEnabled(true);
        ui->verticalSlider_2->setEnabled(true);
        ui->manipBtn->setEnabled(true);
    }
}

void Widget::RCDisconnected()
{
    QMessageBox::warning(this, "Error", "Connection lost!");
    initialized = false;
    ui->connectBtn->setEnabled(true);
    ui->connectBtn->setText("Connect");
    ui->horizontalSlider->setEnabled(false);
    ui->verticalSlider->setEnabled(false);
    ui->verticalSlider_2->setEnabled(false);
    ui->manipBtn->setEnabled(false);
}

void Widget::LoadConfig(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "Couldn't read config file!");
        return;
    }
    QTextStream in(&file);
    ui->ipEdit->setText(in.readLine());
    ui->portSpinBox->setValue(in.readLine().toInt());
    file.close();
}

void Widget::on_manipBtn_clicked()
{
    if (ui->openManipRadio->isChecked())
        rc.OpenManip(ui->manipSpin->value());
    else
        rc.CloseManip(ui->manipSpin->value());
}

void Widget::on_autoPitchCheck_stateChanged(int arg1)
{
    if(initialized)
    {
        rc.SetDepthReg(bool(arg1));
    }
}

void Widget::on_autoHeightCheck_stateChanged(int arg1)
{
    int value = ui->verticalSlider->value();
    double depth = int(value / 25.5) + 10;
    if(initialized)
    {
        rc.SetManualControl(! bool(arg1));
        rc.SetTargetDepth(depth);
        rc.SetDepthReg(bool(arg1));
    }
    ui->heightLabel->setText("Высота");
    if(ui->autoHeightCheck->isChecked())
        ui->heightLabel->setText(QString(tr("Высота\n%1 м")).arg(depth));
}
