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

#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QtGui>
#include <QTextEdit>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include "squarebutton.h"

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    //Constructor:
    explicit SaveDialog(QWidget *parent = nullptr);
    ~SaveDialog();

signals:
    saveDetailsReady(QStringList details);

private slots:
    void saveButtonClicked();
    void cancelButtonClicked();

private:
    Ui::SaveDialog *ui;

    QGraphicsScene *m_Scene;
    QLineEdit *m_FileNameLineEdit;
    QTextEdit *m_DescriptionTextEdit;
};

#endif // SAVEDIALOG_H
