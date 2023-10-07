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

#ifndef TAPEHEAD_H
#define TAPEHEAD_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QtGui>
#include <QtCore>

class TapeHead : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    //Constructor:
    explicit TapeHead(QObject *parent = nullptr);
};

#endif // TAPEHEAD_H
