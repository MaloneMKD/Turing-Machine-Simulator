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

#include "tmedge.h"

TMEdge::TMEdge(int from, int toState, QString read, QString write, QString move)
{
    m_Read = read;
    m_Write = write;
    m_Move = move;
    m_ToState = toState;
    m_FromState = from;
}

QString TMEdge::getRead() const
{
    return m_Read;
}

QString TMEdge::getWrite() const
{
    return m_Write;
}

QString TMEdge::getMove() const
{
    return m_Move;
}

int TMEdge::getFromState() const
{
    return m_FromState;
}

int TMEdge::getToState() const
{
    return m_ToState;
}

void TMEdge::setRead(QString read)
{
    m_Read = read;
}

void TMEdge::setWrite(QString write)
{
    m_Write = write;
}

void TMEdge::setMove(QString move)
{
    m_Move = move;
}

void TMEdge::setFromState(int state)
{
    m_FromState = state;
}

void TMEdge::setToState(int toState)
{
    m_ToState = toState;
}
