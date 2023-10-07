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

#include "pixmapbutton.h"
#include <QDebug>

PixmapButton::PixmapButton(QObject *parent, QString picA, QString picB):
    QObject(parent)
{
    //Settings:
    this->setAcceptHoverEvents(true);
    this->setTransformOriginPoint(this->sceneBoundingRect().center());

    //Initialize variables:
    picA == "" ? m_PicA = nullptr : m_PicA = new QPixmap(picA);
    picB == "" ? m_PicB = nullptr : m_PicB = new QPixmap(picB);

    if(m_PicA)
        this->setPixmap(*m_PicA);
}

void PixmapButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    //Capture the current scale:
    m_CurrentScale = this->scale();

    //Change the picture:
    if(m_PicB)
        this->setPixmap(*m_PicB);
}

void PixmapButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    //Change the picture:
    if(m_PicA)
        this->setPixmap(*m_PicA);
}

void PixmapButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(m_CurrentScale - (m_CurrentScale * 0.02));
    emit this->clicked();
}

void PixmapButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(m_CurrentScale);
}

QString PixmapButton::getLabel() const
{
    return m_Label;
}

void PixmapButton::setLabel(const QString &newLabel)
{
    m_Label = newLabel;
}
