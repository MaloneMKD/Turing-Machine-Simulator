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

#include "popupmessagebox.h"
#include "ui_popupmessagebox.h"
#include <QDebug>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>

PopUpMessagebox::PopUpMessagebox(QWidget *parent, QString title, QString message, QPixmap backgroundPic) :
    QDialog(parent),
    ui(new Ui::PopUpMessagebox)
{
    ui->setupUi(this);
    this->setFixedSize(400, 350);
    this->setFont(QFont("Corbel Light"));
    this->setWindowTitle("Message");
    this->setModal(true);

    //Set up the scene and view:
    m_Scene = new QGraphicsScene(this);
    m_Scene->setSceneRect(0, 0, 378, 328);
    ui->graphicsView->setScene(m_Scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setFrameStyle(QFrame::Raised);

    //Message box design:

    //Inner box:
    m_Scene->addRect(5, 5, 368, 318, QPen(Qt::darkGray), QBrush(Qt::white));

    //Heading box:
    //m_Scene->addRect(14.5, 15, 350, 30);
    QGraphicsTextItem *messegeTitle = m_Scene->addText(title, QFont("Corbel Light", 14.0, -1, true));
    messegeTitle->setPos(189.5 - messegeTitle->sceneBoundingRect().width()/2, 15);

    //Underline heading:
    m_Scene->addLine(messegeTitle->sceneBoundingRect().x() - 10, messegeTitle->sceneBoundingRect().y() + messegeTitle->sceneBoundingRect().height(),
                     messegeTitle->sceneBoundingRect().x() + messegeTitle->sceneBoundingRect().width() + 8, messegeTitle->sceneBoundingRect().y() + messegeTitle->sceneBoundingRect().height());

    //Messege box:
    //m_Scene->addRect(14.5, 65, 350, 200, QPen(Qt::lightGray));
    QTextEdit *messegeTextEdit = new QTextEdit;
    QGraphicsProxyWidget *textEditProxy = m_Scene->addWidget(messegeTextEdit);
    messegeTextEdit->setFrameStyle(QFrame::Raised);
    messegeTextEdit->setFixedSize(350, 200);
    messegeTextEdit->setText(message);
    messegeTextEdit->setFont(QFont("Corbel Light", 12.0));
    textEditProxy->setPos(14.5, 65);

    //Background:
    QGraphicsPixmapItem *bg = m_Scene->addPixmap(backgroundPic);
    bg->setOpacity(0.20);
    bg->setScale(0.8);
    bg->setPos(115, 150);

    //Exit box:
    m_Scene->addRect(136.5, 285, 101, 31, QPen(Qt::lightGray));
    m_ExitButton = new SquareButton(this, QRect(0, 0, 100, 30), "Close");
    m_ExitButton->setPos(136.5, 285);
    connect(m_ExitButton, SIGNAL(clicked()), this, SLOT(close()));
    m_Scene->addItem(m_ExitButton);
}

PopUpMessagebox::~PopUpMessagebox()
{
    delete ui;
}
