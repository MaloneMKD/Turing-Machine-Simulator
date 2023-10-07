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

#include "tmstate.h"

TMState::TMState(int stateNum, bool isStart, bool isHalt)
{
    m_StateNum = stateNum;
    m_IsHALTState = isHalt;
    m_IsSTARTState = isStart;
    m_NumEdges = 0;
}

int TMState::getStateNum() const
{
    return m_StateNum;
}

int TMState::getNumEdges() const
{
    return m_NumEdges;
}

TMEdge TMState::getEdge(int num)
{
    return m_Edges[num];
}

bool TMState::isHALTState()
{
    return m_IsHALTState;
}

bool TMState::isSTARTState()
{
    return m_IsSTARTState;
}

void TMState::addEdge(TMEdge edge)
{
    m_Edges.append(edge);
    m_NumEdges++;
}


