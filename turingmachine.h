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

#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H

#include "tmstate.h"
#include "tmedge.h"
#include <QList>
#include <QObject>

class TuringMachine : public QObject
{
public:
    //Constructor:
    explicit TuringMachine(QStringList data);

    //Accessor functions:
    TMState getState(int stateNum) const;
    QStringList getSummaryTableData() const;
    int getNumStates();

    //Mutator functions:
    void addState(TMState theState);
    void build();

private:
    QList<TMState> m_Machine;
    QStringList m_Data;
    QStringList m_SummaryTableData;
    int m_NumOfStates;
};

#endif // TURINGMACHINE_H
