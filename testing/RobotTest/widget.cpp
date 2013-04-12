#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    em.setGeometry(this->rect());
    em.SetBackgroundImage(QImage("D:/ROV.png"));
    em.LoadConfig("D:/em.cfg");
    em.show();

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
}

Widget::~Widget()
{
    server->close();
    delete ui;
}

void Widget::newConnectionNotify()
{
    //QMessageBox::warning(this, "Hooray!", "New Connection");
    //timer->start(1000);
    client = server->nextPendingConnection();

    connect(client, SIGNAL(readyRead()), this, SLOT(refresh()));
    refresh();
}

void Widget::refresh()
{
    qDebug() << 1;
    if(!client)
    {
        client = server->nextPendingConnection();

        connect(client, SIGNAL(readyRead()), this, SLOT(refresh()));
        qDebug() << 2;
    }
    qDebug() << 3;
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
    ui->label_2->setText(result);
}
