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

#ifndef LOOPARROW_H
#define LOOPARROW_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include "mystateitem.h"

class MyStateItem;

class LoopArrow : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    enum{Type = UserType + 2};

    //Constructor:
    explicit LoopArrow(QObject *parent = nullptr, MyStateItem *parentState = nullptr);

    //Accessor member functions:
    QString getLabel();
    QString getStatePointed();
    int type() const override;
    QList<qreal> getLineLengths();

    //Mutator member functions:
    void setLabel(QString label);
    void correctLabelHeight();
    void setLineLengths(QList<qreal> l);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    MyStateItem *m_ParentState;
    QGraphicsTextItem *m_Label;
    QLineF m_RotationLineBefore;
    QLineF m_RotationLineAfter;
    QString m_StatePointed;
    QList<qreal> m_LineLengths;
};

#endif // LOOPARROW_H
