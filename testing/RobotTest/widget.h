#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork/QTcpServer>
#include <QTimer>
#include <QTcpSocket>

#include "enginesmap.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;
    EnginesMap *em;
    QTcpServer *server;
    QTcpSocket *client;
    QTimer *timer;
    bool *reverses;
    double *speeds;

    void Parse(QString str);

private slots:
    void newConnectionNotify();

    void refresh();
};

#endif // WIDGET_H
