#include "enginesmap.h"

#include "math.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QString>

EnginesMap::EnginesMap(QWidget *parent) :
    QWidget(parent),
    imageSpecified(false)
{
}

void EnginesMap::LoadConfig(QString path)
{
    QFile configFile(path);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        configFile.setFileName("D:/em.cfg");
        if (!configFile.open(QIODevice::ReadOnly))
            return;
    }
    engines.clear();
    bool readingEngine = false;
    QTextStream in(&configFile);
    QString line;
    while(!in.atEnd())
    {
        Engine newEngine;
        line = in.readLine();
        if(!readingEngine)
        {
            if(line.contains("Engine"))
            {
                readingEngine = true;
            }
        }
        else
        {
            if(line.contains("End"))
            {
                readingEngine = false;
                newEngine.SetSpeed(0);
                newEngine.SetReverse(0);
                engines.push_back(newEngine);
            }
            else
            {
                QString parameter = line.split('=')[0];
                QString value = line.split('=')[1];
                if (parameter.contains("number"))
                    newEngine.SetNumber(value.toInt());
                if (parameter.contains("rect"))
                {
                    QStringList list = value.split(',');
                    double rect[4];
                    for(int i = 0; i < 4; i++)
                        rect[i] = list[i].toDouble();
                    newEngine.SetRect(rect);
                }
            }
        }
    }
}

void EnginesMap::SetBackgroundImage(QImage image)
{
    _backgroundImage = image;
    imageSpecified = true;
}

void EnginesMap::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    paint.setRenderHint(paint.Antialiasing, true);
    paint.fillRect(0, 0, this->width(), this->height(), Qt::gray);
    if(imageSpecified)
        paint.drawImage(QRect(0,0,this->width(),this->height()), _backgroundImage);
    foreach(Engine eng, engines)
        eng.Draw(&paint, this->width(), this->height());
}

void EnginesMap::SetSpeed(int engineNumber, int speed)
{
    for (int i = 0; i < engines.count(); i++)
        if (engines[i].GetNumber() == engineNumber)
            engines[i].SetSpeed(speed);
    this->repaint();
}

void EnginesMap::SetReverse(int engineNumber, bool reverse)
{
    for (int i = 0; i < engines.count(); i++)
        if (engines[i].GetNumber() == engineNumber)
            engines[i].SetReverse(reverse);
    this->repaint();
}

void EnginesMap::Engine::Draw(QPainter *paint, int fieldWidth, int fieldHeight)
{
    QBrush prevBrush(paint->brush());
    QPen prevPen(paint->pen());
    QRect newRect = QRect(rect[0] * fieldWidth, rect[1] * fieldHeight,
                          rect[2] * fieldWidth, rect[3] * fieldHeight);
    double r = qMin(newRect.width() * 0.7, newRect.height() * 0.7);
    QPoint center(newRect.left() + newRect.width()/2, newRect.top() + newRect.height()/2);
    r = r * 2 / sqrt(2) * 0.7;
    QRect fontRect(center.x() - r / 2, center.y() - r / 2, r, r);

    QColor lightColor, normalColor;
    if(reverse)
    {
        lightColor.setRgb(255, 187, 33);
        normalColor.setRgb(255, 88, 0);
    }
    else
    {
        lightColor.setRgb(33, 181, 229);
        normalColor.setRgb(0, 99, 204);
    }
    QBrush brush(normalColor);

    paint->setBrush(brush);
    paint->drawEllipse(newRect);

    brush.setColor(lightColor);
    paint->setBrush(brush);
    paint->drawPie(newRect, -270 * 16, -(speed / 100 * 360) * 16);

    brush.setColor(Qt::black);
    paint->setBrush(brush);
    paint->drawEllipse(center, (int)(newRect.width() * 0.35), (int)(newRect.height() * 0.35));

    QPen pen(Qt::white);
    QFont font("Roboto", fontRect.height() / 3.5);

    paint->setPen(pen);
    paint->setFont(font);
    paint->drawText(fontRect, Qt::AlignCenter, QString::number(speed)+'%');

    paint->setBrush(prevBrush);
    paint->setPen(prevPen);
}

void EnginesMap::Engine::SetRect(double _rect[])
{
    for (int i = 0; i < 4; i++)
        rect[i] = _rect[i];
}

void EnginesMap::Engine::SetSpeed(double _speed)
{
    speed = _speed;
}

void EnginesMap::Engine::SetReverse(bool _reverse)
{
    reverse = _reverse;
}

void EnginesMap::Engine::SetNumber(int _number)
{
    number = _number;
}

int EnginesMap::Engine::GetNumber()
{
    return number;
}
