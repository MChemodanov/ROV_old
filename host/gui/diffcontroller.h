#ifndef DIFFCONTROLLER_H
#define DIFFCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QTimer>

class DiffController : public QObject
{
    Q_OBJECT
public:
    explicit DiffController(QTcpSocket *socket, QObject *parent = 0);

    double GetPitch();

    int GetForwardSpeed();

    int GetBackwardSpeed();

    void SetTargetPitch(double tgPitch);

private:
    QTcpSocket *_socket;
    QTimer timer;
    double pitch, targetPitch;
    int fwSpeed, bwSpeed;

    void QueryPitch();

    void CalcSpeeds();

private slots:
    void timerTick();
    
signals:
    
public slots:
    
};

#endif // DIFFCONTROLLER_H
