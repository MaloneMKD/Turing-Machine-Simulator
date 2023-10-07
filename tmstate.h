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

#ifndef TMSTATE_H
#define TMSTATE_H

#include "tmedge.h"
#include <QString>
#include <QList>

class TMState
{
public:
    //Constructor:
    TMState(int stateNum=0, bool isStart=false, bool isHalt=false);

    //Accessor functions:
    int getStateNum() const;
    int getNumEdges() const;
    TMEdge getEdge(int num);
    bool isHALTState();
    bool isSTARTState();

    //Mutator functions:
    void addEdge(TMEdge edge);

private:
    int m_StateNum;
    int m_NumEdges;
    bool m_IsHALTState;
    bool m_IsSTARTState;
    QList<TMEdge> m_Edges;
};

#endif // TMSTATE_H
