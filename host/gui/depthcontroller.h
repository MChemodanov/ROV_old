#ifndef DEPTHCONTROLLER_H
#define DEPTHCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QTimer>

class DepthController : public QObject
{
    Q_OBJECT
public:
    explicit DepthController(QTcpSocket *socket, QObject *parent = 0);

    double GetDepth();

    int GetVertSpeed();

    void SetTargetDepth(double tgDepth);

private:
    QTcpSocket *_socket;
    QTimer timer;
    double depth, targetDepth;
    int vertSpeed;

    void QueryDepth();

    void CalcSpeed();

private slots:
    void timerTick();
    
signals:
    
public slots:
    
};

#endif // DEPTHCONTROLLER_H
