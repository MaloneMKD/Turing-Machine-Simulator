/*(C)Copyright 2023 Malone Napier-Jameson
 *
 * This file is part of Turing Machine Simulator.
 * Turing Machine Simulator is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * Turing Machine Simulator is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License along with Turing Machine Simulator.
 * There is also a copy available inside the application.
 * If not, see <https://www.gnu.org/licenses/>.
*/

#include "solidarrow.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QMessageBox>

SolidArrow::SolidArrow(QObject *parent, MyStateItem *parentState):
    QObject(parent), m_ParentState(parentState)
{
    //Initialize variables:
    m_FirstMovement = true;
    m_Connected = false;
    m_FMAL = false;
    m_Bent = false;
    m_JustLoaded = false;
    m_PointedState = nullptr;
    m_Orientation = 'l';
    m_StatePointed = "";
    m_Length = 3;
    m_SaveLength = 3;
    m_LabelXPos= 3;
    m_LineLengths.append(5);

    //Set up the polygon:
    QList<QPointF> points;
    points << QPointF(2, m_Length - 3) << QPointF(4, m_Length) << QPointF(2.5, m_Length) << QPointF(2.5, 11.5)
           << QPointF(2.5,20) << QPointF(1.5,20) << QPointF(1.5, 11.5)  << QPointF(1.5,m_Length) << QPointF(0,m_Length);
    this->setPolygon(QPolygonF(points));
    this->setScale(3.0);
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    this->setRotation(this->rotation());

    //Set pen and brush:
    this->setPen(QPen(Qt::black, 0.12));
    this->setBrush(Qt::black);

    //Set up the label:
    m_Label = new QGraphicsTextItem("a,a,L", this);
    m_Label->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    m_Label->setScale(0.3);
    m_Label->setRotation(this->rotation() + 90);
    m_Label->setPos(m_LabelXPos - 1.0, m_Length/2.0 + 6);
    m_Label->setFlag(QGraphicsItem::ItemStacksBehindParent);
    m_Label->installEventFilter(this);

    //Initialize connection circle and point:
    m_TipPoint.setX(2.0);
    m_TipPoint.setY(0.0);

    //Initialize lines:
    m_RotationLineBefore.setLength(0);
    m_RotationLineAfter.setLength(0);
    m_RotationLineBefore.setP1(this->sceneBoundingRect().center());
    m_RotationLineBefore.setP2(QPointF(this->sceneBoundingRect().center().x(), this->sceneBoundingRect().center().y() - 20));
    m_RotationLineAfter = m_RotationLineBefore;
}

QString SolidArrow::getStatePointedto()
{
    return m_StatePointed;
}

qreal SolidArrow::getLength()
{
    return m_SaveLength;
}

QList<qreal> SolidArrow::getLineLengths()
{
    return m_LineLengths;
}

QPointF SolidArrow::getTipPoint()
{
    return this->mapToScene(m_TipPoint);
}

MyStateItem *SolidArrow::getPointedStatePointer()
{
    return m_PointedState;
}

QString SolidArrow::getLabel()
{
    return m_Label->toPlainText();
}

int SolidArrow::type() const
{
    //Enable the use of qgraphicsitem_cast<>() with this object:
    return Type;
}

bool SolidArrow::isBent()
{
    return m_Bent;
}

bool SolidArrow::justLoaded()
{
    return m_JustLoaded;
}

void SolidArrow::setConnected(bool val)
{
    m_Connected = val;
}

void SolidArrow::resetRotationLines()
{
    m_RotationLineBefore = QLineF(this->parentItem()->sceneBoundingRect().center(), this->mapToScene(m_TipPoint));
    m_RotationLineAfter = m_RotationLineBefore;
}

void SolidArrow::setFMAL(bool b)
{
    m_FMAL = b;
}

void SolidArrow::bend()
{
    //Bend the arrows:
    QList<QPointF> points;
    points << QPointF(2, m_Length - 3) << QPointF(4, m_Length) << QPointF(2.5, m_Length) << QPointF(-5.5, 11.5)
           << QPointF(2.5,20) << QPointF(1.5,20) << QPointF(-6.5, 11.5)  << QPointF(1.5,m_Length) << QPointF(0,m_Length);
    this->setPolygon(QPolygonF(points));
    m_Bent = true;

    //Position the labels:
    if(m_Orientation == 'l')
    {
        m_LabelXPos = -1.5;
         m_Label->setPos(m_LabelXPos, m_Length/2.0 + 6);
    }
    else if(m_Orientation == 'r')
    {
        m_LabelXPos = -5.0;
        m_Label->setPos(m_LabelXPos + (m_LineLengths.length() * -4.5), m_Length/2.0 + 6);
    }

}

void SolidArrow::straighten()
{
    //Straighten the arrows:
    QList<QPointF> points;
    points << QPointF(2, m_Length - 3) << QPointF(4, m_Length) << QPointF(2.5, m_Length) << QPointF(2.5, 11.5)
           << QPointF(2.5,20) << QPointF(1.5,20) << QPointF(1.5, 11.5)  << QPointF(1.5,m_Length) << QPointF(0,m_Length);
    this->setPolygon(QPolygonF(points));
    m_Bent = false;

    //Position the labels:
    m_LabelXPos = 3;
    m_Label->setPos(m_LabelXPos, m_Length/2.0 + 6);
    m_Label->setScale(0.3);
}

void SolidArrow::setJustLoaded(bool jl)
{
    m_JustLoaded = jl;
}

void SolidArrow::setLineLengths(QList<qreal> lengths)
{
    m_LineLengths = lengths;
}

void SolidArrow::lengthenBy(qreal amount)
{
    //New length:
    m_Length += amount/3.0;
    m_SaveLength += amount/3.0;

    //Update polygon dimensions:
    QList<QPointF> points;
    points << QPointF(2, m_Length - 3) << QPointF(4, m_Length) << QPointF(2.5, m_Length) << QPointF(2.5, 11.5+m_Length)
           << QPointF(2.5,20) << QPointF(1.5,20) << QPointF(1.5, 11.5+m_Length)  << QPointF(1.5,m_Length) << QPointF(0,m_Length);
    this->setPolygon(QPolygonF(points));

    //Update label position and size:
    if(m_Orientation == 'r')
        m_Label->setPos(m_LabelXPos, m_Length/2.0 + 10);
    else
        m_Label->setPos(m_LabelXPos - 1.0, m_Length/2.0 + 6);

    if(m_SaveLength > -20 && amount > 0 && m_Label->scale() > 0.18)
        m_Label->setScale(m_Label->scale() - 0.002);
    else if(m_SaveLength > -20 && amount < 0 && m_Label->scale() <= 0.3)
        m_Label->setScale(m_Label->scale() + 0.002);
    else if(m_SaveLength < -20 && amount < 0)
        m_Label->setScale(0.3);

    //Update the tip point:
    QPolygonF p = this->polygon();
    m_TipPoint.setY(p.first().y());
}

void SolidArrow::setStatePointed(QString state)
{
    m_StatePointed = state;
}

void SolidArrow::setColor(QColor color)
{
    this->setPen(QPen(color, 0.12));
}

void SolidArrow::setLabel(QString label)
{
    m_Label->setPlainText(label);
}

void SolidArrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{    
    //Set Rotation:
    m_RotationLineBefore.setP1(m_RotationLineAfter.p1());
    m_RotationLineBefore.setP2(m_RotationLineAfter.p2());
    m_RotationLineAfter.setP1(m_ParentState->sceneBoundingRect().center());
    m_RotationLineAfter.setP2(event->scenePos());
    this->setRotation(this->rotation() - m_RotationLineBefore.angleTo(m_RotationLineAfter));

    //Set label rotation:
    QLineF arrowAngleLine(m_ParentState->sceneBoundingRect().center(), this->sceneBoundingRect().center());
    if((arrowAngleLine.angle() < 90 || arrowAngleLine.angle() > 270) && m_Orientation == 'l')
    {
        m_Label->setRotation(m_Label->rotation() + 180);
        m_Orientation = 'r';
    }
    else if((arrowAngleLine.angle() > 90 && arrowAngleLine.angle() < 270) && m_Orientation == 'r')
    {
        m_Label->setRotation(m_Label->rotation() + 180);
        m_Orientation = 'l';
    }

    //Resize:
    qreal diff = m_RotationLineBefore.length() - m_RotationLineAfter.length();

    if(m_FirstMovement)
    {
        diff = 0;
        m_FirstMovement = false;
        if(m_FMAL)
        {
            //Reset the rotation lines if this is the first time moving the arrow after loading:
            this->resetRotationLines();
            m_FMAL = false;
        }
    }
    this->lengthenBy(diff);

    //Check if the arrow has left a state:
    if(m_StatePointed != "")
    {
        QList<QGraphicsItem*> colliding = this->collidingItems();

        //Filter off all items that are not states:
        for(int i = 0; i < colliding.length(); i++)
        {
            if(!qgraphicsitem_cast<MyStateItem*>(colliding[i]) || colliding[i]->isAncestorOf(this))
                colliding.removeAt(i);
        }

        //Go through all colliding states see if m_StatePointedTo matches any of the states:
        bool stillPointing = false;
        for(int i = 0; i < colliding.length(); i++)
        {
            MyStateItem *temp = qgraphicsitem_cast<MyStateItem*>(colliding[i]);
            if(temp && temp->getStateName() == m_StatePointed)
            {
                stillPointing = true;
                break;
            }
        }
        if(!stillPointing)
            emit this->iLeft(this);
    }
}

bool SolidArrow::eventFilter(QObject *watched, QEvent *event)
{
    //If the is a key press event on the arrow:
    if(watched == m_Label && event->type() == QEvent::KeyPress && m_Bent && m_Orientation == 'r')
    {
        qreal offset = 4.5;
        //Get the key event:
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        //If the key event is <ENTER>:
        if(keyEvent && keyEvent->key() == Qt::Key_Return)
        {
            if(m_LineLengths.length() < 6)
            {
                QPointF p = this->mapFromScene(m_Label->scenePos());
                m_Label->setPos(p.x() - offset, p.y());
                m_LineLengths.append(0);
                return false;
            }
            else
                return true;
        }
        //If the key is <BACKSPACE>:
        else if(keyEvent && keyEvent->key() == Qt::Key_Backspace)
        {
            if(m_LineLengths[m_LineLengths.length()-1] == 0 && m_LineLengths.length() > 1)
            {
                QPointF p = this->mapFromScene(m_Label->scenePos());
                m_Label->setPos(p.x() + offset, p.y());
                m_LineLengths.pop_back();
                return false;
            }
            else
            {
                if(m_LineLengths.length() > 0 && m_LineLengths.last() > 0)
                    m_LineLengths[m_LineLengths.length()-1]--;
                return false;
            }
        }
        //If the key is any other valid key:
        else if(keyEvent->key() >= 0x20 && keyEvent->key()<= 0x7e && keyEvent->key() != Qt::Key_Return)
        {
            m_LineLengths[m_LineLengths.length() - 1]++;
        }
        else
            return false;
    }
    else if(watched == m_Label && event->type() == QEvent::KeyPress && m_Bent && m_Orientation == 'l')
    {
        //Get the key event:
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        //If the key event is <ENTER>:
        if(keyEvent && keyEvent->key() == Qt::Key_Return)
        {
            if(m_LineLengths.length() < 6)
            {
                m_LineLengths.append(0);
                return false;
            }
            else
                return true;
        }
        //If the key is <BACKSPACE>:
        else if(keyEvent && keyEvent->key() == Qt::Key_Backspace)
        {
            if(m_LineLengths[m_LineLengths.length()-1] == 0 && m_LineLengths.length() > 1)
            {
                m_LineLengths.pop_back();
                return false;
            }
            else
            {
                if(m_LineLengths.length() > 0 && m_LineLengths.last() > 0)
                    m_LineLengths[m_LineLengths.length()-1]--;
                return false;
            }
        }
        //If the key is any other valid key:
        else if(keyEvent->key() >= 0x20 && keyEvent->key()<= 0x7e && keyEvent->key() != Qt::Key_Return)
        {
            m_LineLengths[m_LineLengths.length() - 1]++;
        }
        else
            return false;
    }
    return QObject::eventFilter(watched, event);
}

void SolidArrow::readjust()
{
    if(m_PointedState)
    {
        //Draw line from center of the state to the point on the other state:
        QLineF l1(m_ParentState->sceneBoundingRect().center(), m_PointedState->getConnectionPoint(this));
        this->setRotation(90 - l1.angle());

        //Calculate the difference to the connection point:
        this->expandTo(m_PointedState->getConnectionPoint(this));
    }
}

void SolidArrow::setPointedState(MyStateItem *state)
{
    m_PointedState = state;
}

void SolidArrow::expandTo(QPointF p)
{
    qreal beforeLength = m_SaveLength;
    //Get the arrow polygon and the connection point:
    QPolygonF polygon = this->polygon();
    QPointF point = this->mapFromScene(p);

    //Change the coordinates in order to lengthen the arrow:
    polygon[0] = QPointF(point.x(), point.y());
    polygon[1] = QPointF(point.x() + 2, point.y() + 3);
    polygon[2] = QPointF(point.x() + 0.5, point.y() + 3);
    if(!m_Bent)
    {
        polygon[3] = QPointF(point.x() + 0.5, point.y() + 3);
        polygon[6] = QPointF(point.x() - 0.5, point.y() + 3);
    }
    polygon[7] = QPointF(point.x() - 0.5, point.y() + 3);
    polygon[8] = QPointF(point.x() - 2, point.y() + 3);

    m_SaveLength = point.y();
    this->setPolygon(polygon);

    //Set label rotation:
    QLineF arrowAngleLine(m_ParentState->sceneBoundingRect().center(), this->sceneBoundingRect().center());
    if((arrowAngleLine.angle() < 90 || arrowAngleLine.angle() > 270) && m_Orientation == 'l')
    {
        m_Label->setRotation(m_Label->rotation() + 180);
        m_Orientation = 'r';
        if(m_Bent)
            m_LabelXPos = -5.0;
    }
    else if((arrowAngleLine.angle() > 90 && arrowAngleLine.angle() < 270) && m_Orientation == 'r')
    {
        m_Label->setRotation(m_Label->rotation() + 180);
        m_Orientation = 'l';
        if(m_Bent)
            m_LabelXPos = -1.5;
    }

    //Update label position:
    qreal distance = m_SaveLength - beforeLength;
    if(m_SaveLength > -20 && distance > 0 && m_Label->scale() > 0.18)
        m_Label->setScale(m_Label->scale() - 0.002);
    else if(m_SaveLength > -20 && distance < 0 && m_Label->scale() <= 0.3)
        m_Label->setScale(m_Label->scale() + 0.002);
    else if(m_SaveLength < -20 && distance < 0)
        m_Label->setScale(0.3);

    if(m_Orientation == 'r')
    {
        if(m_Bent)
            m_Label->setPos(m_LabelXPos + (m_LineLengths.length() * -4.5), m_SaveLength/2.0 + 10);
        else
            m_Label->setPos(m_LabelXPos, m_SaveLength/2.0 + 12);
    }
    else
        m_Label->setPos(m_LabelXPos - 1.0, m_SaveLength/2.0 + 8);
}

void SolidArrow::snapToPosition(MyStateItem *state)
{
    while(state->containsTip(this->mapToItem(state, m_TipPoint)))
        this->lengthenBy(0.2);
    this->lengthenBy(-0.5);
    this->resetRotationLines();
}
