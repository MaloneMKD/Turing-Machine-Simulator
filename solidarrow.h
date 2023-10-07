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

#ifndef SOLIDARROW_H
#define SOLIDARROW_H

#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QtCore>
#include <QtGui>
#include "mystateitem.h"

class MyStateItem;

class SolidArrow : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    enum {Type = UserType + 1};

    //Constructor:
    SolidArrow(QObject *parent, MyStateItem *parentState);

    //Accessor member functions:
    QString getStatePointedto();
    QString getLabel();
    qreal getLength();
    QList<qreal> getLineLengths();
    QPointF getTipPoint();
    MyStateItem* getPointedStatePointer();
    int type() const override;
    bool isBent();
    bool justLoaded();

    //Mutator member functions:
    void lengthenBy(qreal amount);
    void setStatePointed(QString state);
    void setColor(QColor color);
    void setLabel(QString label);
    void snapToPosition(MyStateItem* state);
    void readjust();
    void setPointedState(MyStateItem* state);
    void expandTo(QPointF p);
    void setConnected(bool val);
    void resetRotationLines();
    void setFMAL(bool b);
    void bend();
    void straighten();
    void setJustLoaded(bool jl);
    void setLineLengths(QList<qreal> lengths);

signals:
    void iLeft(SolidArrow* a);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    MyStateItem *m_ParentState;
    MyStateItem *m_PointedState;
    QGraphicsTextItem *m_Label;
    QList<qreal> m_LineLengths;
    QLineF m_RotationLineBefore;
    QLineF m_RotationLineAfter;
    QLineF m_StateToPoint;
    qreal m_Length;
    qreal m_SaveLength;
    qreal m_LabelXPos;
    QString m_StatePointed;
    QPointF m_TipPoint;
    QChar m_Orientation;
    bool m_FirstMovement;
    bool m_Connected;
    bool m_FMAL;//First movement after loading
    bool m_Bent;
    bool m_JustLoaded;
};

#endif // SOLIDARROW_H
