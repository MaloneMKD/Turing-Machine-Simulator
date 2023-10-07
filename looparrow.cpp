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

#include "looparrow.h"
#include <QGraphicsScene>

LoopArrow::LoopArrow(QObject *parent,MyStateItem *parentState)
    : QObject{parent}, m_ParentState(parentState), m_StatePointed(parentState->getStateName())
{
    //Setup polygon and settings:
    QList<QPointF> points;
    points << QPointF(0, 6) << QPointF(0, 0) << QPointF(6, 0) << QPointF(3.2, 5) << QPointF(3.8, 5.5) <<
              QPointF(2, 6) << QPointF(1.8, 4.1) << QPointF(2.5, 4.6) << QPointF(4.5, 1) << QPointF(1, 1) << QPointF(1, 6);

    this->setPolygon(QPolygonF(points));
    this->setScale(5.0);
    this->setPen(QPen(Qt::cyan, 0.12));
    this->setBrush(Qt::black);
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemIsSelectable);

    //Initialize lines:
    m_RotationLineBefore.setLength(0);
    m_RotationLineAfter.setLength(0);

    //Set up the label:
    m_Label = new QGraphicsTextItem("a,a,L", this);
    m_Label->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_Label->setScale(0.18);
    m_Label->setPos(0,this->sceneBoundingRect().top() - 3.1);
    m_Label->setFlag(QGraphicsItem::ItemStacksBehindParent);
    m_Label->installEventFilter(this);

    //Initialize variables:
    m_LineLengths.append(5);
}

QString LoopArrow::getLabel()
{
    return m_Label->toPlainText();
}

QString LoopArrow::getStatePointed()
{
    return m_StatePointed;
}

int LoopArrow::type() const
{
    //Enable the use of qgraphicsitem_cast<>() with this object:
    return Type;
}

QList<qreal> LoopArrow::getLineLengths()
{
    return m_LineLengths;
}

void LoopArrow::setLabel(QString label)
{
    m_Label->setPlainText(label);
}

void LoopArrow::correctLabelHeight()
{
    for(int i = 0; i < m_LineLengths.length() - 1; i++)
    {
        QPointF p = this->mapFromScene(m_Label->scenePos());
        m_Label->setPos(p.x(), p.y() - 2.9);
    }
}

void LoopArrow::setLineLengths(QList<qreal> l)
{
    m_LineLengths = l;
}

void LoopArrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //Set Rotation:
    m_RotationLineBefore.setP1(m_RotationLineAfter.p1());
    m_RotationLineBefore.setP2(m_RotationLineAfter.p2());
    m_RotationLineAfter.setP1(m_ParentState->sceneBoundingRect().center());
    m_RotationLineAfter.setP2(event->scenePos());
    this->setRotation(this->rotation() - m_RotationLineBefore.angleTo(m_RotationLineAfter));
}

bool LoopArrow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_Label && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        //If the key pressed is <ENTER>:
        if(keyEvent && keyEvent->key() == Qt::Key_Return)
        {
            if(m_LineLengths.length() < 6)
            {
                QPointF p = this->mapFromScene(m_Label->scenePos());
                m_Label->setPos(p.x(), p.y() - 2.9);
                m_LineLengths.append(0);
                return false;
            }
            else
                return true;
        }
        else if(keyEvent && keyEvent->key() == Qt::Key_Backspace)
        {
            if(m_LineLengths[m_LineLengths.length()-1] == 0 && m_LineLengths.length() > 1)
            {
                QPointF p = this->mapFromScene(m_Label->scenePos());
                m_Label->setPos(p.x(), p.y() + 2.9);
                m_LineLengths.pop_back();
                return false;
            }
            else
            {
                if(m_LineLengths.last() > 0)
                    m_LineLengths[m_LineLengths.length()-1]--;
            }
        }
        else if(keyEvent->key() >= 0x20 && keyEvent->key()<= 0x7e && keyEvent->key() != Qt::Key_Return)
        {
            m_LineLengths[m_LineLengths.length() - 1]++;
        }
        else
            return false;
    }
    return QObject::eventFilter(watched, event);
}
