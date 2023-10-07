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

#include "turingmachine.h"
#include <QStringList>
#include <QString>
#include <QDebug>

TuringMachine::TuringMachine(QStringList data): m_Data(data), m_NumOfStates(0)
{
}

TMState TuringMachine::getState(int stateNum) const
{
    if(stateNum >= 0 && stateNum < m_Machine.length() && !m_Machine.isEmpty())
        return m_Machine[stateNum];

    return TMState();
}

QStringList TuringMachine::getSummaryTableData() const
{
    return m_SummaryTableData;
}

int TuringMachine::getNumStates()
{
    return m_NumOfStates;
}

void TuringMachine::addState(TMState theState)
{
    m_Machine.append(theState);
}

void TuringMachine::build()
{
    //Create each state:
    for(int i = 0; i < m_Data.length(); i++)
    {
        QString state = m_Data[i];
        QString ss = state[0];
        QString hs = state[2];

        //Variables to check if the state is a halt or start state:
        bool isHALTState = false;
        bool isSTARTState = false;

        //Check if HALT state:
        if(hs == "1")
            isHALTState = true;
        else if(hs == "0")
            isHALTState = false;
        //Check if START state:
        if(ss == "1")
            isSTARTState = true;
        else if(ss == "0")
            isSTARTState = false;

        //Remove start/halt indicators:
        state.remove(0,4);

        if(!isHALTState)
        {
            QStringList edges = state.split('_', Qt::SkipEmptyParts);
            QString stateNum = edges[0][1];
            TMState tempState(stateNum.toInt(), isSTARTState, isHALTState);

            for(int j = 0; j < edges.length(); j++)
            {
                //Append the edge data to a variable for the summary table:
                m_SummaryTableData.append(edges[j]);

                //Create an edge and append it t the state:
                QString from = edges[j][1];
                QString to = edges[j][4];
                TMEdge tempEdge(from.toInt(),
                                to.toInt(),
                                edges[j][6],
                                edges[j][8],
                                edges[j][10]);
                tempState.addEdge(tempEdge);
            }
            m_Machine.append(tempState);
        }
        else
        {
            QString stateNum = state[1];
            TMState tempState(stateNum.toInt(), false, isHALTState);
            m_Machine.append(tempState);

            //Append halt state data to the summary table data:
            QString data = "q" + stateNum + ",H ,A,L,T";
            m_SummaryTableData.append(data);
        }
        m_NumOfStates++;
    }
}
