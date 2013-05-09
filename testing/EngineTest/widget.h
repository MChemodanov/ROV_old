#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDial>
#include <QTimer>
#include <QtNetwork/QTcpSocket>

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
    QSpinBox *enginesSpin,
                *portSpin;
    QLineEdit *ipaddressEdit;
    QPushButton *connectBtn;
    QCheckBox *reverseCheck;
    QDial *speedDial;
    QTcpSocket socket;
    QTimer timer;
    int lastEngine;
    Ui::Widget *ui;

    struct EngineData
    {
        QVector<int> newSpeed,
                actualSpeed,
                newReverse,
                actualReverse,
                ticksSinceLastReverse;
        QVector<double> angle,
                        coeff;
    } ed;

    void LoadConfig(QString path);

    void InitLayout();

    void WriteSpeed(int speed, int engine);

    void WriteReverse(int reverse, int engine);

private slots:

    void connectBtn_clicked();
    void timer_ticked();
    void data_changed();
    void enginesSpin_changed();
};

#endif // WIDGET_H
