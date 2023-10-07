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

#include "squarespawnbox.h"
#include "mystateitem.h"
#include <QPainter>

SquareSpawnBox::SquareSpawnBox(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent):
    QGraphicsRectItem(x, y, width, height, parent)
{
}

void SquareSpawnBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    bool collidesWithState = false;

    //Get all colliding items:
    QList<QGraphicsItem*> cl = this->collidingItems();

    //Check if any of the colliding items is a state:
    for(int i = 0; i < cl.length(); i++)
    {
        if(qgraphicsitem_cast<MyStateItem*>(cl[i]))
        {
            collidesWithState = true;
            break;
        }
    }

    //If collidesWithState, change color, else don't:
    if(collidesWithState)
    {

        painter->setOpacity(0.5);
        this->update();
    }
    else
    {
        painter->setBrush(Qt::white);
        painter->setOpacity(0.3);
        this->update();
    }

    //Paint the rect:
    painter->drawRect(this->rect());
}

