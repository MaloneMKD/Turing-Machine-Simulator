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

#ifndef TMEDGE_H
#define TMEDGE_H

#include <QString>

//Forward declaration of the TMState class:

class TMEdge
{
public:
    //Constructors and destructor;
    TMEdge(int from = 1, int toState = 1, QString read = "a", QString write = "a", QString move = "S");

    //Accessor functions:
    QString getRead() const;
    QString getWrite() const;
    QString getMove() const;
    int getFromState() const;
    int getToState() const;

    //Mutator member functions:
    void setRead(QString read);
    void setWrite(QString write);
    void setMove(QString move);
    void setFromState(int state);
    void setToState(int toState);

private:
    QString m_Read;
    QString m_Write;
    QString m_Move;
    int m_FromState;
    int m_ToState;
};

#endif // TMEDGE_H
