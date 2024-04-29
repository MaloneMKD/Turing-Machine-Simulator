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

#include <QFont>
#include "squaretapecell.h"
#include <QPen>

SquareTapeCell::SquareTapeCell(QRectF rect)
    : QGraphicsRectItem(rect, nullptr)
{
    //Initialize label:
    m_Label = new QGraphicsTextItem("a", this);
   /* m_Label->setPos(this->sceneBoundingRect().center().x() - this->scenePos().x(),
                    this->sceneBoundingRect().center().y() - this->scenePos().y());*/
    m_Label->setPos(this->rect().x(), this->rect().y() - m_Label->sceneBoundingRect().height() / 2.5);
    m_Label->setScale(3.0);
    QFont font = m_Label->font();
    font.setPointSizeF(4.0);
    m_Label->setFont(font);
    this->setPen(QPen(Qt::darkGray, 1.5));
}

void SquareTapeCell::setLabel(QString label)
{
    m_Label->setPlainText(label);
}

