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

#include "squarebutton.h"
#include <QFont>
#include <QRadialGradient>

SquareButton::SquareButton(QObject *parent, QRectF rect, QString label, QColor textHighlightColor, QColor buttonHighlightColor)
    : QObject(parent), QGraphicsRectItem(rect)
{
    //Settings and flags:
    this->setAcceptHoverEvents(true);
    this->setTransformOriginPoint(this->sceneBoundingRect().center());
    this->setPen(QPen(Qt::gray));
    m_TextHighlightColor = textHighlightColor;
    m_ButtonHighlightColor = buttonHighlightColor;

    //Set up outter rect:
    m_InnerRect = new QGraphicsRectItem(this->sceneBoundingRect().x() + 2, this->sceneBoundingRect().y() + 3.5, rect.width() - 2, rect.height() - 6, this);
    m_InnerRect->setTransformOriginPoint(m_InnerRect->sceneBoundingRect().center());
    m_InnerRect->setPen(QPen(Qt::white));
    m_InnerRect->setBrush(QBrush(Qt::white));

    //Set up label:
    m_Label = new QGraphicsTextItem(label, this);
    m_Label->setPos(m_InnerRect->sceneBoundingRect().width()/2 + m_InnerRect->sceneBoundingRect().x() - m_Label->sceneBoundingRect().width()/2 + 1,
                    m_InnerRect->sceneBoundingRect().height()/2 + m_InnerRect->sceneBoundingRect().y() - m_Label->sceneBoundingRect().height()/2 - 1);
    QFont font = m_Label->font();
    font.setPointSizeF(8.0);
    m_Label->setFont(font);
    m_Label->setDefaultTextColor(Qt::black);
}

void SquareButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(0.95);
    m_InnerRect->setScale(0.95);
    emit this->clicked();
}

void SquareButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setScale(1.0);
    m_InnerRect->setScale(1.0);
}

void SquareButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setBrush(QBrush(m_ButtonHighlightColor));
    this->setScale(1.0);
    m_InnerRect->setScale(1.0);
    m_Label->setDefaultTextColor(m_TextHighlightColor);
}

void SquareButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setBrush(QBrush(Qt::white));
    m_Label->setDefaultTextColor(Qt::black);
}
