#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QtNetwork/QTcpSocket>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    reverseCheck(NULL),
    speedDial(NULL),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->gridLayoutWidget->setGeometry(0, 0, this->width(), this->height());

    ipaddressEdit = new QLineEdit(this);

    portSpin = new QSpinBox(this);
    portSpin->setMaximum(25565);

    enginesSpin = new QSpinBox(this);
    enginesSpin->setMinimum(1);
    enginesSpin->setValue(6);
    connect(enginesSpin, SIGNAL(valueChanged(int)), this, SLOT(enginesSpin_changed()));

    connectBtn = new QPushButton("Connect", this);
    connect(connectBtn, SIGNAL(clicked()), this, SLOT(connectBtn_clicked()));

    connect(&timer, SIGNAL(timeout()), this, SLOT(timer_ticked()));

    LoadConfig("rc.cfg");

    ui->gridLayout = new QGridLayout(this);

    ui->gridLayout->addWidget(ipaddressEdit, 0, 0);
    ui->gridLayout->addWidget(portSpin, 0, 1);
    ui->gridLayout->addWidget(enginesSpin, 0, 2);
    ui->gridLayout->addWidget(connectBtn, 0, 3);

    ed.newSpeed.resize(enginesSpin->value());
    ed.actualSpeed.resize(enginesSpin->value());
    ed.newReverse.resize(enginesSpin->value());
    ed.actualReverse.resize(enginesSpin->value());
    ed.ticksSinceLastReverse.resize(enginesSpin->value());

    InitLayout();

    ui->gridLayoutWidget->setLayout(ui->gridLayout);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::InitLayout()
{
    if(reverseCheck != NULL)
    {
        for (int i = 0; i < enginesSpin->value(); i++)
            ui->gridLayout->removeWidget(&reverseCheck[i]);
        delete[] reverseCheck;
        reverseCheck = NULL;
    }
    if(speedDial != NULL)
    {
        for (int i = 0; i < enginesSpin->value(); i++)
            ui->gridLayout->removeWidget(&speedDial[i]);
        delete[] speedDial;
        speedDial = NULL;
    }

    reverseCheck = new QCheckBox[enginesSpin->value()]();

    speedDial = new QDial[enginesSpin->value()];

    for (int i = 0; i < enginesSpin->value(); i++)
    {
        reverseCheck[i].setText("R");
        reverseCheck[i].setEnabled(false);
        connect(&reverseCheck[i], SIGNAL(stateChanged(int)), this, SLOT(data_changed()));

        speedDial[i].setEnabled(false);
        speedDial[i].setMinimum(0);
        speedDial[i].setMaximum(255);
        speedDial[i].setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        connect(&speedDial[i], SIGNAL(valueChanged(int)), this, SLOT(data_changed()));

        ui->gridLayout->addWidget(&speedDial[i], 2, i);
        ui->gridLayout->addWidget(&reverseCheck[i], 3, i);
    }
}

void Widget::connectBtn_clicked()
{
    socket.connectToHost(ipaddressEdit->text(), portSpin->value());

    timer.start(500);

    for(int i = 0; i < enginesSpin->value(); i++)
    {
        reverseCheck[i].setEnabled(true);
        speedDial[i].setEnabled(true);
    }

    connectBtn->setEnabled(false);
}

void Widget::data_changed()
{
    for(int i = 0; i < enginesSpin->value(); i++)
    {
        ed.newSpeed[i] = speedDial[i].value();
        ed.newReverse[i] = reverseCheck[i].isChecked();
    }
}

void Widget::enginesSpin_changed()
{
    ed.newSpeed.resize(enginesSpin->value());
    ed.actualSpeed.resize(enginesSpin->value());
    ed.newReverse.resize(enginesSpin->value());
    ed.actualReverse.resize(enginesSpin->value());
    ed.ticksSinceLastReverse.resize(enginesSpin->value());
    InitLayout();
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
    ipaddressEdit->setText(in.readLine());
    portSpin->setValue(in.readLine().toInt());
    file.close();
}

void Widget::timer_ticked()
{
    int ticksForReverse = 4;

    for(int i = 0; i < enginesSpin->value(); i++)
        ed.ticksSinceLastReverse[i]++;

    lastEngine++;

        if(lastEngine >= enginesSpin->value())
            lastEngine = 0;

        if (ed.ticksSinceLastReverse[lastEngine] >= ticksForReverse &&
                ed.newReverse[lastEngine] != ed.actualReverse[lastEngine])
        {
            ed.ticksSinceLastReverse[lastEngine] = 0;
            ed.actualReverse[lastEngine] = ed.newReverse[lastEngine];
            WriteReverse(ed.actualReverse[lastEngine], lastEngine);
        }else
        if (ed.newSpeed[lastEngine] != ed.actualSpeed[lastEngine])
        {
            ed.actualSpeed[lastEngine] = ed.newSpeed[lastEngine];
            WriteSpeed(ed.actualSpeed[lastEngine], lastEngine);
        }
}

void Widget::WriteSpeed(int speed, int engine)
{
    QString toWrite = "#p" + QString::number(engine) + QString::number(speed) + '!';
    if(socket.isWritable())
        socket.write(toWrite.toStdString().c_str());
}

void Widget::WriteReverse(int reverse, int engine)
{
    QString toWrite ="#r" + QString::number(engine) + QString::number(reverse) + '!';
    if(socket.isWritable())
        socket.write(toWrite.toStdString().c_str());
}
