#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    depth(0),
    pitch(0)
{
    ui->setupUi(this);

    em.setGeometry(this->rect());
    em.SetBackgroundImage(QImage("ROV.png"));
    em.LoadConfig("em.cfg");
    ui->gridLayout->addWidget(&em, 0, 0);
    ui->gridLayoutWidget->setLayout(ui->gridLayout);

    server = new QTcpServer();
    server->connect(server, SIGNAL(newConnection()), this, SLOT(newConnectionNotify()));

    QHostAddress address("127.0.0.1");
    if(!server->listen(address, 6666))
    {
        QMessageBox::warning(this, "Error", "Server can't listen");
        ui->label->setText(server->errorString());
    }
    else
        ui->label->setText(server->serverAddress().toString() + ':' + QString::number(server->serverPort()));

    reverses = new bool[6];
    speeds = new double[6];
    for (int i = 0; i < 6; i++)
    {
        reverses[i] = false;
        speeds[i] = 0;
    }

    connect(&timer, SIGNAL(timeout()), this, SLOT(CalcData()));
    timer.start(1000);
}

Widget::~Widget()
{
    server->close();
    delete ui;
}

void Widget::newConnectionNotify()
{
    client = server->nextPendingConnection();

    connect(client, SIGNAL(readyRead()), this, SLOT(refresh()));
    refresh();
}

void Widget::refresh()
{
    if(!client)
    {
        client = server->nextPendingConnection();

        connect(client, SIGNAL(readyRead()), this, SLOT(refresh()));
    }
    char buffer[1024] = {0};
    if(client->isReadable())
        client->read(buffer, client->bytesAvailable());

    //ui->label_2->setText(buffer);
    QString str(buffer);
    Parse(str);
}

void Widget::Parse(QString str)
{
    QStringList list = str.split('!');
    foreach(QString element, list)
    {
        if (element == "#qp")
            client->write(QString("$qp%1!").arg(pitch).toStdString().c_str());
        if (element == "#qd")
            client->write(QString("$qd%1!").arg(depth).toStdString().c_str());

        int i = 0;
        if (element[0] != '#')
            continue;
        if (element[1] == 'p' || element[1] == 'r')
            i = QString(element[2]).toInt();
        else
            continue;
        if (element[1] == 'p')
            speeds[i] = element.mid(3).toInt();
        else
            reverses[i] = element.mid(3).toInt() == 1 ? true : false;
    }
    QString result = "";
    for (int i = 0; i < 6; i++)
    {
        result += QString("Engine #%1 speed: %2 reverse %3\n").arg(i).arg(speeds[i]).arg(reverses[i]);
        em.SetSpeed(i, speeds[i]/2.55);
        em.SetReverse(i, reverses[i]);
    }
    result += QString("Depth: %1\nPitch: %2").arg(depth).arg(pitch);
    ui->label_2->setText(result);
}

void Widget::CalcData()
{
    depth += ((reverses[0]?1:-1) * speeds[0] + (reverses[1]?1:-1) * speeds[1]) / 1000;
    pitch += ((reverses[0]?-1:1) * speeds[0] - (reverses[1]?-1:1) * speeds[1]) / 100;
}

void Widget::on_pushButton_clicked()
{
    depth = ui->doubleSpinBox->value();
}

void Widget::on_pushButton_2_clicked()
{
    pitch = ui->doubleSpinBox_2->value();
}
