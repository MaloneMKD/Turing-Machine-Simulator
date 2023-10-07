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

#ifndef MYSTATEITEM_H
#define MYSTATEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QtCore>
#include <QtGui>
#include <QObject>
#include "solidarrow.h"
#include "looparrow.h"

class SolidArrow;
class LoopArrow;

class MyStateItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    enum{Type = UserType + 3};
    enum Status {Ready, ArrowNotPointingToState, ArrowLabelInvalid};

    //Constructor:
    MyStateItem(QObject *parent, QGraphicsItem *parentItem, QString label);

    //Accessor member functions:
    QString getStateName() const;
    QString getStateData() const;
    QStringList getSaveData() const;
    QPointF getConnectionPoint(const SolidArrow *s) const;
    bool isSTARTState() const;
    bool isHALTState() const;
    Status readyForProcessing() const;
    bool hasArrows() const;
    bool hasLoopArrow() const;
    bool containsTip(QPointF point);
    int type() const override;

    //Mutator member functions:
    void createStraightArrow();
    void createLoopArrow();
    void createStraightArrow(SolidArrow *s);
    void createLoopArrow(LoopArrow *l, qreal lRotation);
    void deleteArrow(QGraphicsItem *arrow);
    void detachFromPointedArrows();
    void detachMyArrowsFromOtherStates();
    void decrementStateNameNum(int stateNum);
    void setIsSTARTState();
    void setIsHALTState();
    void setConnectionPoint(const SolidArrow *s, QPointF p);
    void changeColor(QColor color);
    void setConnectedArrowColor(QColor color);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private slots:
    void arrowLeft(SolidArrow *a);

private:
    QList<SolidArrow*> m_Arrows;
    QList<SolidArrow*> m_ArrowsPointingToThis;
    QList<QPointF> m_ConnectionPoints;
    QRegularExpression m_LabelPattern;
    LoopArrow *m_LoopArrow;
    QGraphicsTextItem *m_Label;
    QString m_LabelString;
    QPointF translatedPoint;
    QColor m_BrushColor;
    QColor m_ConnectedArrowColor;
    bool m_IsHALTState;
    bool m_IsSTARTState;
    bool m_HasLoopArrow;
};

#endif // MYSTATEITEM_H
