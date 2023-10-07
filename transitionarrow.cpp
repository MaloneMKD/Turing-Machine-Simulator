#include "transitionarrow.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

TransitionArrow::TransitionArrow(QObject *parent, MyStateItem *parentItem, QPointF headCenter, int x, int y, int width, int height):
                                 QObject(parent), m_ParentState(parentItem)
{
    //Size and color:
    m_Color = Qt::black;
    this->setRect(x, y, width, height);
    this->setBrush(QBrush(m_Color));
    this->setPen(QPen(m_Color));


    //Set transformation origin point:
    this->setTransformOriginPoint(parentItem->boundingRect().center().x(), parentItem->boundingRect().y()+25);

    //Initialize trackers:
    m_BeforeLine.setP1(m_ParentState->boundingRect().center());
    m_BeforeLine.setP2(headCenter);
    m_AfterLine.setP1(m_ParentState->boundingRect().center());
    m_AfterLine.setP2(headCenter);
}

void TransitionArrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Change color:
    m_Color = Qt::cyan;
    this->setBrush(QBrush(m_Color));
    this->setPen(QPen(m_Color));

    m_ParentState->scene()->addLine(QLineF(this->transformOriginPoint(), this->boundingRect().bottomLeft()));
}

void TransitionArrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void TransitionArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_Color = Qt::black;
    this->setBrush(QBrush(m_Color));
    this->setPen(QPen(m_Color));
}

void TransitionArrow::followHead(QPointF newPos)
{
    m_BeforeLine.setP1(m_AfterLine.p1());
    m_BeforeLine.setP2(m_AfterLine.p2());
    m_AfterLine.setP2(newPos);

    this->setRotation(this->rotation() - m_BeforeLine.angleTo(m_AfterLine));
}
