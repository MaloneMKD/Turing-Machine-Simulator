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

#include "tapecell.h"
#include <QGraphicsScene>

TapeCell::TapeCell(QObject *parent, QRectF rect)
    : QGraphicsRoundedRectItem(parent, nullptr, rect)
{
    //Initialize label:
    m_Label = new QGraphicsTextItem("a", this);
    m_Label->setPos(this->sceneBoundingRect().center().x() - 16,
                    this->sceneBoundingRect().center().y()/1.8);
    m_Label->setScale(3.0);
    QFont font = m_Label->font();
    font.setPointSizeF(4.0);
    m_Label->setFont(font);
}

void TapeCell::setLabel(QString label)
{
    m_Label->setPlainText(label);
}
