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

#ifndef POPUPMESSAGEBOX_H
#define POPUPMESSAGEBOX_H

#include <QDialog>
#include <QGraphicsScene>
#include <QtGui>
#include <QtCore>
#include <QTextEdit>
#include <squarebutton.h>

namespace Ui {
class PopUpMessagebox;
}

class PopUpMessagebox : public QDialog
{
    Q_OBJECT

public:
    explicit PopUpMessagebox(QWidget *parent = nullptr, QString title = "Title", QString message = "Message", QPixmap backgroundPic = QPixmap());
    ~PopUpMessagebox();

private:
    Ui::PopUpMessagebox *ui;
    QGraphicsScene *m_Scene;
    QTextEdit *m_MessegeEdit;
    SquareButton *m_ExitButton;
};

#endif // POPUPMESSAGEBOX_H
