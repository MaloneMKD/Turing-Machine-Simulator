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

#include "mystateitem.h"
#include <QDebug>
#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QRegularExpression>

MyStateItem::MyStateItem(QObject *parent, QGraphicsItem *parentItem, QString label):
                        QObject(parent), QGraphicsEllipseItem(parentItem)
{
    //Initialize variables:
    m_LabelString= label;
    m_LoopArrow = nullptr;
    m_IsHALTState = false;
    m_IsSTARTState = false;
    m_HasLoopArrow = false;
    m_BrushColor = Qt::white;
    m_ConnectedArrowColor = Qt::cyan;
    m_LabelPattern = QRegularExpression("([0-9]|[A-Z]|[a-z]|$|#|-){1,1},([0-9]|[A-Z]|[a-z]|$|#|-){1,1},(r|R|l|L|s|S){1,1}$");

    //Set up:
    this->setRect(10,10,50,50);
    this->setPen(QPen(Qt::black));
    this->setBrush(m_BrushColor);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    this->setAcceptHoverEvents(true);

    //Set up label:
    m_Label = new QGraphicsTextItem(label, this);
    m_Label->setScale(1.0);
    m_Label->setPos(this->sceneBoundingRect().width()/2, this->sceneBoundingRect().height()/2);
}

QString MyStateItem::getStateName() const
{
    return m_LabelString;
}

bool MyStateItem::isSTARTState() const
{
    return m_IsSTARTState;
}

bool MyStateItem::isHALTState() const
{
    return m_IsHALTState;
}

MyStateItem::Status MyStateItem::readyForProcessing() const
{
    //Check that every arrow is pointing at a state:
    for(int i = 0; i < m_Arrows.length(); i++)
    {
        if(m_Arrows[i]->getStatePointedto() == "")
            return MyStateItem::ArrowNotPointingToState;
    }

    //If all arrows are pointing to a state, check to see the validity of the labels:
    QRegularExpressionMatch reMatch;
    for(SolidArrow *s : m_Arrows)
    {
        QStringList labelList = s->getLabel().split('\n', Qt::SkipEmptyParts);
        for(int i = 0; i < labelList.length(); i++)
        {
            //If the string is a null string:
            if(labelList[i] == "" || labelList[i] == QString())
                return MyStateItem::ArrowLabelInvalid;

            reMatch = m_LabelPattern.match(labelList[i]);
            if(!reMatch.hasMatch())
                return MyStateItem::ArrowLabelInvalid;
        }
    }

    //Test the Loop arrow label;
    if(m_LoopArrow != nullptr)
    {
        reMatch = m_LabelPattern.match(m_LoopArrow->getLabel());
        if(!reMatch.hasMatch())
            return MyStateItem::ArrowLabelInvalid;
    }
    return MyStateItem::Ready;
}

bool MyStateItem::hasArrows() const
{
    if(m_Arrows.length() == 0)
        return false;
    return true;
}

bool MyStateItem::hasLoopArrow() const
{
    return m_HasLoopArrow;
}

bool MyStateItem::containsTip(QPointF point)
{
    if(this->contains(point))
        return true;
    return false;
}

int MyStateItem::type() const
{
    return Type;
}

void MyStateItem::createStraightArrow()
{
    if(!m_IsHALTState)
    {
        //Create new arrow on the heap:
        SolidArrow *m_Arrow = new SolidArrow(this, this);
        m_Arrow->setParentItem(this);
        m_Arrows.append(m_Arrow);

        //Set its transformation origin point:
        m_Arrow->setTransformOriginPoint(this->sceneBoundingRect().center().x() - this->scenePos().x() - this->sceneBoundingRect().width()/2.0 - 7.0,
                                         this->sceneBoundingRect().center().y() - this->scenePos().y() - 6.5);
        m_Arrow->setPos(this->sceneBoundingRect().width() / 2.0 +  7.0, 6.5);
    }
}

void MyStateItem::createLoopArrow()
{
    if(!m_IsHALTState)
    {
        //Create new loop arrow on the heap:
        if(m_LoopArrow == nullptr)
        {
            m_LoopArrow = new LoopArrow(this, this);
            m_LoopArrow->setParentItem(this);
            m_LoopArrow->setTransformOriginPoint(this->sceneBoundingRect().center().x() - this->scenePos().x() - m_LoopArrow->sceneBoundingRect().width() - 2,
                                                 this->sceneBoundingRect().center().y() - this->scenePos().y() - m_LoopArrow->sceneBoundingRect().height()/2 - 8.7);
            m_LoopArrow->setPos(m_LoopArrow->sceneBoundingRect().width() + 2,
                                m_LoopArrow->sceneBoundingRect().height()/2 + 8.7);
            m_LoopArrow->setPen(QPen(m_ConnectedArrowColor, 0.12));
            m_HasLoopArrow = true;
        }
    }
}

void MyStateItem::createStraightArrow(SolidArrow *s)
{
    if(s != nullptr)
    {
        s->setParentItem(this);
        m_Arrows.append(s);

        //Set its transformation origin point:
        s->setTransformOriginPoint(this->sceneBoundingRect().center().x() - this->scenePos().x() - this->sceneBoundingRect().width()/2.0 - 7.0,
                                         this->sceneBoundingRect().center().y() - this->scenePos().y() - 6.5);
        s->setPos(this->sceneBoundingRect().width() / 2.0 +  7.0,
                        6.5);
    }
}

void MyStateItem::createLoopArrow(LoopArrow *l, qreal lRotation)
{
    //Create new loop arrow on the heap:
    if(l != nullptr)
    {
        m_LoopArrow = l;
        m_LoopArrow->setParentItem(this);
        m_LoopArrow->setTransformOriginPoint(this->sceneBoundingRect().center().x() - this->scenePos().x() - l->sceneBoundingRect().width() - 2,
                                             this->sceneBoundingRect().center().y() - this->scenePos().y() - l->sceneBoundingRect().height()/2 - 8.7);
        m_LoopArrow->setPos(l->sceneBoundingRect().width() + 2,
                            l->sceneBoundingRect().height()/2 + 8.7);
        m_LoopArrow->setRotation(lRotation);
        m_LoopArrow->setPen(QPen(m_ConnectedArrowColor, 0.12));
        m_HasLoopArrow = true;
    }
}

void MyStateItem::deleteArrow(QGraphicsItem *arrow)
{
    //Check if arrow is the loop arrow:
    if(arrow == m_LoopArrow)
    {
        this->scene()->removeItem(arrow);
        delete m_LoopArrow;
        m_LoopArrow = nullptr;
        m_HasLoopArrow = false;
    }
    else
    {
        //Signal the states pointed to that the arrow is leaving:
        SolidArrow *temp = qgraphicsitem_cast<SolidArrow*>(arrow);
        emit temp->iLeft(temp);
        this->scene()->removeItem(arrow);

        //Find out which of the arrows was deleted:
        int index = -1;
        for(int i = 0; i < m_Arrows.length(); i++)
        {
            if(arrow == m_Arrows[i])
                index = i;
        }

        //Remove the arrow from the list;
        if(index >= 0)
        {
            delete m_Arrows[index];
            m_Arrows.removeAt(index);
        }
    }
}

void MyStateItem::detachFromPointedArrows()
{
    //Disconnect from pointing arrows and change their linking variables:
    for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
    {
        m_ArrowsPointingToThis[i]->setStatePointed("");
        m_ArrowsPointingToThis[i]->setColor(Qt::black);
        disconnect(m_ArrowsPointingToThis[i], SIGNAL(iLeft(SolidArrow*)), this, SLOT(arrowLeft(SolidArrow*)));
    }
}

void MyStateItem::detachMyArrowsFromOtherStates()
{
    for(int i = 0; i < m_Arrows.length(); i++)
        emit m_Arrows[i]->iLeft(m_Arrows[i]);
}

void MyStateItem::decrementStateNameNum(int stateNum)
{
    if(!m_IsHALTState)
    {
        m_LabelString = QString("q%1") .arg(stateNum - 1);
        m_Label->setPlainText(m_LabelString);
    }
}

void MyStateItem::setIsSTARTState()
{
    //Change the label:
    m_IsSTARTState = true;
    m_LabelString += "\nSTART";
    m_Label->setPlainText(m_LabelString);

    //Set label size:
    QFont font = m_Label->font();
    font.setPointSizeF(7.0);
    m_Label->setFont(font);
    m_Label->setPos(m_Label->pos().x() - 5, m_Label->pos().y() - 5);
}

void MyStateItem::setIsHALTState()
{
    //Change the label:
    m_IsHALTState = true;
    m_LabelString += "\nHALT";
    m_Label->setPlainText(m_LabelString);

    //Set label size:
    QFont font = m_Label->font();
    font.setPointSizeF(7.0);
    m_Label->setFont(font);
    m_Label->setPos(m_Label->pos().x() - 5, m_Label->pos().y() - 5);

    //Update the name in all pointing states:
    for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
        m_ArrowsPointingToThis[i]->setStatePointed(m_LabelString);
}

void MyStateItem::setConnectionPoint(const SolidArrow *s, QPointF p)
{
    //m_ConnectionPoint = this->mapFromScene(p);
    for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
    {
        if(s == m_ArrowsPointingToThis[i])
            m_ConnectionPoints[i] = this->mapFromScene(p);
    }
}

void MyStateItem::changeColor(QColor color)
{
    m_BrushColor = color;
    this->setBrush(m_BrushColor);
}

void MyStateItem::setConnectedArrowColor(QColor color)
{
    m_ConnectedArrowColor = color;

    for(SolidArrow *s : m_ArrowsPointingToThis)
        s->setColor(color);
    if(m_LoopArrow)
        m_LoopArrow->setPen(QPen(color, 0.12));
}

QString MyStateItem::getStateData() const
{
    QString entry = "";
    //Indicate whether the state is a start state or not:
    if(m_IsSTARTState)
        entry.append("1_");
    else
        entry.append("0_");

    //Remove extra words for processing reasons:
    QString from = m_LabelString;
    QString to = "";
    if(from.contains("START"))
        from.remove("\nSTART");
    if(from.contains("HALT"))
        from.remove("\nHALT");

    //Indicate whether the state is a halt state or not:
    if(m_IsHALTState)
        entry.append(QString("1_%1") .arg(from));
    else
        entry.append("0_");

    //Loop arrow:
    if(m_LoopArrow != nullptr)
    {
        //Get all possible lables separated by a foward slash:
        QStringList allDat = m_LoopArrow->getLabel().split('\n');

        //Make an entry for each label on the arrow:
        for(int i = 0; i < allDat.length(); i++)
            entry +=  from + ',' + from + ',' + allDat[i] + '_';
    }

    //Regular arrows:
    for(int i = 0; i < m_Arrows.length(); i++)
    {
        to = m_Arrows[i]->getStatePointedto();
        if(to.contains("HALT"))
            to.remove("\nHALT");
        if(to.contains("START"))
            to.remove("\nSTART");

        //Get all possible lables separated by a foward slash:
        QStringList allDat = m_Arrows[i]->getLabel().split('\n');

        //Make an entry for each label on the arrow:
        for(int j = 0; j < allDat.length(); j++)
            entry += from + ',' + to + ',' + allDat[j] + '_';
    }
    return entry;
}

QStringList MyStateItem::getSaveData() const
{
    QStringList dataList;
    QString label = m_LabelString;

    //Get the name of the state:
    if(label.contains("\nSTART"))
        label.remove("\nSTART");
    if(label.contains("\nHALT"))
        label.remove("\nHALT");
    dataList.append(label);

    //Indicate whehter start state or not:
    if(isSTARTState())
        dataList.append("true");
    else
        dataList.append("false");

    //Indicate whether halt state or not:
    if(isHALTState())
        dataList.append("true");
    else
        dataList.append("false");

    //Get the state's scene position:
    QString scenePos = QString("%1,%2") .arg(this->scenePos().x()) .arg(this->scenePos().y());
    dataList.append(scenePos);

    //Get data for every edge:
    if(m_LoopArrow != nullptr)
    {
        //Indicate the number of edges:
        dataList.append(QString::number(m_Arrows.length() + 1));
        dataList.append("++");
        //Get and save the loop label:
        QString loopLabel = m_LoopArrow->getLabel();
        if(loopLabel.contains("\nSTART"))
            loopLabel.remove("\nSTART");
        loopLabel.replace('\n', '/');
        dataList.append(loopLabel);
        //Get and save the lines length variabel:
        QString lengthRec = "";
        for(qreal r : m_LoopArrow->getLineLengths())
            lengthRec.append(QString("%1,") .arg(r));
        dataList.append(lengthRec);
        //Get and save the state pointed to:
        QString pt = m_LoopArrow->getStatePointed();
        if(pt.contains("\nSTART"))
            pt.remove("\nSTART");
        dataList.append(pt);
        //Save the rotation:
        dataList.append(QString::number(m_LoopArrow->rotation()));
    }
    else
    {
        //Indicate the number of edges:
        dataList.append(QString::number(m_Arrows.length()));
        dataList.append("--");
    }

    QString statePointed;
    QString arrLabel;
    for(int i = 0; i < m_Arrows.length(); i++)
    {
        //Add the label to the data list:
        arrLabel = m_Arrows[i]->getLabel();
        arrLabel.replace('\n', '/');
        dataList.append(arrLabel);

        //Add the m_LineLengths:
        QString lengths = "";
        for(qreal length : m_Arrows[i]->getLineLengths())
            lengths += QString::number(length) + ',';
        dataList.append(lengths);

        //Add the bent state to the data list:
        if(m_Arrows[i]->isBent())
            dataList.append("bent");
        else
            dataList.append("notBent");

        //Add the state pointed to to the data list:
        statePointed = m_Arrows[i]->getStatePointedto();
        if(statePointed.contains("\nSTART"))
            statePointed.remove("\nSTART");
        if(statePointed.contains("\nHALT"))
            statePointed.remove("\nHALT");

        dataList.append(statePointed);        
        dataList.append(QString::number(m_Arrows[i]->getLength()));
        dataList.append(QString::number(m_Arrows[i]->rotation()));
    }
    return dataList;
}

QPointF MyStateItem::getConnectionPoint(const SolidArrow *s) const
{
    for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
    {
        if(s == m_ArrowsPointingToThis[i])
            return this->mapToScene(m_ConnectionPoints[i]);
    }
    return QPointF();
}

void MyStateItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    //Change color:
    this->setBrush(Qt::black);
    m_Label->setDefaultTextColor(Qt::white);

    QList<QGraphicsItem*> colliderList = this->collidingItems();

    //Filter out all non arrow items:
    for(int i = 0; i < colliderList.length(); i++)
    {
        if(!qgraphicsitem_cast<SolidArrow*>(colliderList[i]))
            colliderList.removeAt(i);
    }

    //Check if the hover enter event brought in new arrows:
    for(QGraphicsItem* a : colliderList)
    {
        SolidArrow *temp = qgraphicsitem_cast<SolidArrow*>(a);
        //Check if item is a SolidArrow and  if the arrow is from a different state:
        if(temp && !this->isAncestorOf(temp))
        {
            //Check if the arrow has already been noted:
            bool isAccountedFor = false;
            for(SolidArrow *s : m_ArrowsPointingToThis)
            {
                if(temp == s)
                    isAccountedFor = true;
            }
            //If the arrow has not been noted, note it:
            if(!isAccountedFor && qobject_cast<QObject*>(temp))
            {
                temp->setPointedState(this);
                temp->setStatePointed(m_LabelString);
                temp->setColor(m_ConnectedArrowColor);
                temp->setConnected(true);
                temp->snapToPosition(this);
                m_ArrowsPointingToThis.append(temp);
                m_ConnectionPoints.append(this->mapFromScene(temp->getTipPoint()));
                connect(temp, SIGNAL(iLeft(SolidArrow*)), this, SLOT(arrowLeft(SolidArrow*)));

                //Check if any of the arrows in this state are pointing to the state that the new arrow comes from:
                MyStateItem *p = qgraphicsitem_cast<MyStateItem*>(temp->parentItem());
                for(SolidArrow *s: m_Arrows)
                {
                    if(s->getStatePointedto() == p->getStateName())
                    {
                        s->bend();
                        if(!s->justLoaded())
                            s->expandTo(p->getConnectionPoint(s));
                        else
                            s->setJustLoaded(false);
                        temp->bend();
                        break;
                    }
                }
            }
        }
    }
}

void MyStateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    //Change color:
    this->setBrush(m_BrushColor);
    m_Label->setDefaultTextColor(Qt::black);

    if(!m_IsHALTState)
    {
        //Get all colliding items:
        QList<QGraphicsItem*> collidingItems = this->collidingItems();

        //Filter all colliding items to just arrows:
        for(int i = 0; i < collidingItems.length(); i++)
        {
            if(!qgraphicsitem_cast<SolidArrow*>(collidingItems[i]))
                collidingItems.removeAt(i);
        }

        //Check if any of the noted arrows are no longer colliding with the state,
        //remove all such arrows from the list:
        bool isStillColliding;
        SolidArrow *temp = nullptr;
        for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
        {
            isStillColliding = false;
            for(QGraphicsItem* q : collidingItems)
            {
                temp = qgraphicsitem_cast<SolidArrow*>(q);
                if(m_ArrowsPointingToThis[i] == temp)
                {
                    isStillColliding = true;
                    break;
                }
            }

            //Remove arrows that are no longer colliding with the state:
            if(!isStillColliding && temp)
            {
                m_ArrowsPointingToThis[i]->setStatePointed("");
                m_ArrowsPointingToThis[i]->setColor(Qt::black);
                m_ArrowsPointingToThis[i]->setConnected(false);
                m_ArrowsPointingToThis[i]->setPointedState(nullptr);

                //Remove the arrow and disconnect from it:
                m_ArrowsPointingToThis.removeAt(i);
                m_ConnectionPoints.removeAt(i);
                disconnect(temp, SIGNAL(iLeft(SolidArrow*)), this, SLOT(arrowLeft(SolidArrow*)));
            }
        }
    }
}

QVariant MyStateItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged)
    {
        //Call readjust() on all state arrows:
        foreach(SolidArrow *g, m_Arrows)
        {
            if(g->getPointedStatePointer() != nullptr)
            {
                //Calculate the new point and the angle to it:
                QLineF dLine(g->getPointedStatePointer()->sceneBoundingRect().center(), g->getPointedStatePointer()->getConnectionPoint(g));
                QLineF tLine(g->getPointedStatePointer()->sceneBoundingRect().center(), this->sceneBoundingRect().center());
                dLine.setAngle(tLine.angle());

                //Get the point at the new line and set m_ConnectionPoint to that point
                g->getPointedStatePointer()->setConnectionPoint(g, dLine.p2());
                g->readjust();

                //Update rotation lines because arrow rotation has changed:
                g->resetRotationLines();
            }
        }

        //Update position of arrows pointing to this:
        for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
        {
            //Calculate the new point and the angle to it:
            QLineF dLine(this->sceneBoundingRect().center(), this->mapToScene(m_ConnectionPoints[i]));
            QLineF tLine(this->sceneBoundingRect().center(), m_ArrowsPointingToThis[i]->parentItem()->sceneBoundingRect().center());
            dLine.setAngle(tLine.angle());

            //Get the point at the new line and set m_ConnectionPoint to that point
            m_ConnectionPoints[i] = this->mapFromScene(dLine.p2());
            m_ArrowsPointingToThis[i]->readjust();
            m_ArrowsPointingToThis[i]->resetRotationLines();
        }
    }    
    return  QGraphicsItem::itemChange(change, value);
}

void MyStateItem::arrowLeft(SolidArrow *a)
{
    //Find the arrow that left:
    for(int i = 0; i < m_ArrowsPointingToThis.length(); i++)
    {
        if(m_ArrowsPointingToThis[i] == a)
        {
            //If the arrow being removed is bent then we must straighten both it and its counterpart on this state:
            if(a->isBent())
            {
                MyStateItem *s = qgraphicsitem_cast<MyStateItem*>(a->parentItem());
                for(SolidArrow *ar: m_Arrows)
                {
                    if(ar->getStatePointedto() == s->getStateName() && ar->isBent())
                    {
                        ar->straighten();
                        a->straighten();

                        ar->expandTo(s->getConnectionPoint(ar));
                        break;
                    }
                }
            }

            //Disconnect the arrow that left and reset it to default settings:
            disconnect(a, SIGNAL(iLeft(SolidArrow*)), this, SLOT(arrowLeft(SolidArrow*)));
            m_ArrowsPointingToThis.removeAt(i);
            m_ConnectionPoints.removeAt(i);
            a->setColor(Qt::black);
            a->setStatePointed("");
            a->setPointedState(nullptr);
        }
    }
}
