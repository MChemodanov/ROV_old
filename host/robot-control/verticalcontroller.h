#ifndef VERTICALCONTROLLER_H
#define VERTICALCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "auto_ptr.h"

#include "depthcontroller.h"
#include "diffcontroller.h"

class VerticalController : public QObject
{
    Q_OBJECT
public:
    explicit VerticalController(QTcpSocket *socket, QObject *parent = 0);

    void SetTargetDepth(double depth);

    void SetTargetPitch(double pitch);

    int GetForwardSpeed();

    int GetBackwardSpeed();

    double GetDepth();

    double GetPitch();

    void SetPitchReg(bool state);

    void SetDepthReg(bool state);

private:
    QTimer timer;
    std::auto_ptr<DepthController> depthc;
    std::auto_ptr<DiffController> diffc;
    QTcpSocket *socket;
    double targetDepth, targetPitch,
            realDepth, realPitch,
            fwSpeed, bwSpeed;
    bool pitchReg, depthReg;

    void CalcData();

private slots:
    void TimerTick();

    
signals:
    
public slots:
    
};

#endif // VERTICALCONTROLLER_H
