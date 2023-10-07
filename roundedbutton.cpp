#include <QFont>
#include <QtCore>
#include <QtGui>
#include "roundedbutton.h"

RoundedButton::RoundedButton(QObject *parent, QGraphicsItem *parentItem, QRectF rect, QString label)
    : QGraphicsRoundedRectItem{parent, parentItem, rect}
{
    //Settings and flags:
    //this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setAcceptHoverEvents(true);

    //Setup label:
    m_Label = new QGraphicsTextItem(label, this);
    m_Label->setPos(this->sceneBoundingRect().width()/2 + this->sceneBoundingRect().x() - m_Label->sceneBoundingRect().width()/2,
                    this->sceneBoundingRect().height()/2 + this->sceneBoundingRect().y() - m_Label->sceneBoundingRect().height()/2 - 1);
    QFont font = m_Label->font();
    font.setPointSizeF(7.8);
    m_Label->setFont(font);
}

void RoundedButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(0.95);
    emit this->clicked();
}

void RoundedButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(1.0);
}

void RoundedButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    QRadialGradient gradient(this->sceneBoundingRect().center().x(), this->sceneBoundingRect().center().y(), 80);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.2, Qt::white);
    gradient.setColorAt(0.4, Qt::white);
    gradient.setColorAt(0.6, Qt::darkCyan);
    this->setBrush(QBrush(gradient));
}

void RoundedButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setBrush(QBrush(Qt::white));
}
