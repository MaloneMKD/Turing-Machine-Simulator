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

#ifndef TMPROCESSOR_H
#define TMPROCESSOR_H

#include "turingmachine.h"
#include <QObject>
#include <QString>

class TMProcessor : QObject
{
    Q_OBJECT

public:

    enum ProcessResult{Successful, PossibleInfiniteLoop};

    //Constructor:
    TMProcessor(QObject *parent);

    //Mutator Member Functions:
    ProcessResult start();
    void setParameters(QString input, TuringMachine *theTM);
    void write(QString output);
    void move(QString move);
    void crash();
    void accept();

    //Accessor member functions:
    QStringList getTapeData() const;
    QStringList getTapeRecord() const;
    QList<int> getMachineData() const;
    QString getCrashString() const;
    QString getTransitionRecord() const;

private:
    QString m_InputString;
    QString m_TransitionRecord;
    QString m_CrashString;
    QStringList m_TapeData;
    QStringList m_TapeRecord;
    QList<int> m_MachineData;
    TuringMachine *m_TM;
    int m_CurrentState;
    int m_CurrentInput;
    bool m_Crashed;
    bool m_Accepted;
};

#endif // TMPROCESSOR_H
