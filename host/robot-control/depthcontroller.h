#ifndef DEPTHCONTROLLER_H
#define DEPTHCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class DepthController : public QObject
{
    Q_OBJECT
public:
    explicit DepthController(QTcpSocket *socket, QObject *parent = 0);

    double GetDepth();

    double GetVertSpeed();

    void SetTargetDepth(double tgDepth);

    void QueryDepth();

private:
    QTcpSocket *_socket;
    double depth, targetDepth;
    double vertSpeed;

    void CalcSpeed();
    
signals:
    
public slots:
    
};

#endif // DEPTHCONTROLLER_H
