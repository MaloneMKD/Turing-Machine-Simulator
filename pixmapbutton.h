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

#ifndef PIXMAPBUTTON_H
#define PIXMAPBUTTON_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

class PixmapButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    //Constructor:
    PixmapButton(QObject *parent = nullptr, QString picA = QString(), QString picB = QString());

    //Accessor member functions:
    QString getLabel() const;

    //Mutator member functions:
    void setLabel(const QString &newLabel);

signals:
    void clicked();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QPixmap *m_PicA;
    QPixmap *m_PicB;
    QString m_Label;
    qreal m_CurrentScale;
};

#endif // PIXMAPBUTTON_H
