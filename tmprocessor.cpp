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

#include "tmprocessor.h"
#include "tmstate.h"
#include <QDebug>

TMProcessor::TMProcessor(QObject *parent):
    QObject(parent), m_CurrentState(0)
{
    m_CrashString = "";
}

TMProcessor::ProcessResult TMProcessor::start()
{
    if(m_TM != NULL && m_TM->getNumStates() > 0)
    {
        /* Algorithm:
         *
         * Step 1: Check if current state is a HALT state
         *         If state is HALT state, send accepted signal
         * Step 2: Read an input letter
         * Step 3: Check if edge exists
         * Step 4: If the edge exists, read the write variable and send signal
         *         If not exist, crash.
         * Step 5: Read the move variable of the edge and send signal
         * Step 6: Read the state to which the edge points and update current state variable
         * Step 7: Repeat till all input has been processed
        */

        //Clear variables:
        m_TapeData.clear();
        m_MachineData.clear();
        m_TapeRecord.clear();
        m_CrashString = "";
        m_TransitionRecord = "";

        //Look for start state and set it to current state:
        for(int i = 0; i < m_TM->getNumStates(); i++)
        {
            if(m_TM->getState(i).isSTARTState())
            {
                m_CurrentState = i;
                break;
            }
        }

        m_CurrentInput = 0;
        m_Crashed = false;
        m_Accepted = false;
        bool edgeFound = false;
        bool beyondTapeLimits = false;
        int loopCount = 0;

        //Test every letter in the input string:
        while(!m_Crashed && !m_Accepted && loopCount < 100000)
        {
            TMState tempState = m_TM->getState(m_CurrentState);
            m_MachineData.append(m_CurrentState);

            //If the state is a HALT state, accept input:
            if(tempState.isHALTState())
            {
                this->accept();
                break;
            }

            //If the current state is not a HALT state, process:
            //Check if the current state has an edge with the same character on its read as the letter on the tape:
            edgeFound = false;
            for(int j = 0; j < tempState.getNumEdges(); j++)
            {
                if(m_CurrentInput > -1 && (tempState.getEdge(j).getRead() == m_InputString[m_CurrentInput]))
                {
                    m_TransitionRecord.append(QString("q%1,q%2,%3,%4,%5\n") .arg(m_CurrentState) .arg(tempState.getEdge(j).getToState())
                                                                         .arg(tempState.getEdge(j).getRead()) .arg(tempState.getEdge(j).getWrite())
                                                                         .arg(tempState.getEdge(j).getMove()));
                    edgeFound = true;
                    this->write(tempState.getEdge(j).getWrite());
                    this->move(tempState.getEdge(j).getMove());
                    m_CurrentState = tempState.getEdge(j).getToState();
                    if(m_CurrentInput < 0)
                        beyondTapeLimits = true;
                    break;
                }
            }

            //If the tape head tried to move too far to the left:
            if(beyondTapeLimits)
            {
                m_CrashString.append(QString("The tape head tried to move passed the left end of the tape"));
                this->crash();
                break;
            }

            //If the edge was not found:
            if(!edgeFound)
            {
                if(!edgeFound)
                    m_CrashString.append(QString("State q%1 has no edge with read parameter = \'%2\'")
                                             .arg(tempState.getStateNum()).arg(m_InputString[m_CurrentInput]));
                this->crash();
                break;
            }
            loopCount++;
        }
        if(loopCount >= 100000)
            return PossibleInfiniteLoop;
        else
            return Successful;
    }
}

void TMProcessor::setParameters(QString input, TuringMachine *theTM)
{
    m_InputString = input;
    m_TM = theTM;
}

QStringList TMProcessor::getTapeData() const
{
    return m_TapeData;
}

QStringList TMProcessor::getTapeRecord() const
{
    return m_TapeRecord;
}

QList<int> TMProcessor::getMachineData() const
{
    return m_MachineData;
}

QString TMProcessor::getCrashString() const
{
    return m_CrashString;
}

QString TMProcessor::getTransitionRecord() const
{
    return m_TransitionRecord;
}

void TMProcessor::write(QString output)
{
    m_TapeData.append(output);
    if(m_CurrentInput > -1)
    {
        m_InputString[m_CurrentInput] = output[0];
        m_TapeRecord.append(QString("%1%2") .arg(m_InputString) .arg(m_CurrentInput));
    }
}

void TMProcessor::move(QString move)
{
    m_TapeData[m_TapeData.length() - 1] += move;

    if(move.toLower() == 'l')
        m_CurrentInput--;
    else if(move.toLower() == 'r')
        m_CurrentInput++;
}

void TMProcessor::crash()
{
    m_Crashed = true;
    m_TapeData.append("CRASHED");
    if(m_CurrentInput > -1)
        m_TapeRecord.append(QString("%1%2") .arg(m_InputString) .arg(m_CurrentInput));
}

void TMProcessor::accept()
{
    m_Accepted = true;
    m_TapeData.append("ACCEPTED");
    m_TapeRecord.append(QString("%1%2") .arg(m_InputString) .arg(m_CurrentInput));
}
