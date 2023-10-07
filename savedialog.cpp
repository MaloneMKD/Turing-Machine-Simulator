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

#include "savedialog.h"
#include "ui_savedialog.h"

SaveDialog::SaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialog)
{
    //Setup:
    ui->setupUi(this);
    ui->graphicsView->setFrameStyle(QFrame::Raised);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    this->setFixedSize(465, 350);

    //Setup view and scene:
    m_Scene = new QGraphicsScene(0, 0, 430, 320, this);
    ui->graphicsView->setScene(m_Scene);

    //Setup page:
        //File name:
    //m_Scene->addRect(1, 0, 427, 50, QPen(Qt::gray));
    QGraphicsTextItem *fnLabel = m_Scene->addText("File name: ");
    fnLabel->setDefaultTextColor(Qt::darkGray);
    QFont dialogFont;
    dialogFont.setFamily("Corbel Light");
    dialogFont.setItalic(true);
    dialogFont.setPointSizeF(15.0);
    fnLabel->setFont(dialogFont);
    fnLabel->setPos(2, 1);

    m_Scene->addRect(100, 4, 200, 30, QPen(Qt::gray));
    m_FileNameLineEdit = new QLineEdit;
    m_FileNameLineEdit->setPlaceholderText("Enter a name for your TM here...");
    m_FileNameLineEdit->setFixedSize(200, 30);
    m_FileNameLineEdit->setFont(QFont("Corbel Light", 11.5));
    QGraphicsProxyWidget *lineEditProxy = m_Scene->addWidget(m_FileNameLineEdit);
    lineEditProxy->setPos(100, 4);

    QGraphicsTextItem *ext = m_Scene->addText(".xml");
    ext->setDefaultTextColor(Qt::darkGray);
    ext->setFont(QFont("Corbel Light", 13.0, -1, true));
    ext->setPos(305, 5);

    //Separator line:
    m_Scene->addLine(55, 49 , 362, 49, QPen(Qt::gray));

        //Description:
    //m_Scene->addRect(1, 60, 427, 230, QPen(Qt::gray));
    QGraphicsTextItem *descLabel = m_Scene->addText("Description: ", dialogFont);
    descLabel->setDefaultTextColor(Qt::darkGray);
    descLabel->setPos(2, 55);

    m_Scene->addRect(6, 85, 420, 150, QPen(Qt::gray));
    m_DescriptionTextEdit = new QTextEdit;
    m_DescriptionTextEdit->setPlaceholderText("Enter a description of your TM here...");
    m_DescriptionTextEdit->setFrameStyle(QFrame::Raised);
    m_DescriptionTextEdit->setFont(QFont("Corbel Light", 11));
    m_DescriptionTextEdit->setFixedSize(420, 150);
    QGraphicsProxyWidget *descProxy = m_Scene->addWidget(m_DescriptionTextEdit);
    descProxy->setPos(6, 85);

        //Save and cancel buttons:
    m_Scene->addRect(1, 249, 424, 40, QPen(Qt::gray));
    SquareButton *saveButton = new SquareButton(this, QRectF(2, 250, 212.5, 38), "Save");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
    m_Scene->addItem(saveButton);
    SquareButton *cancelButton = new SquareButton(this, QRectF(211.5, 250, 212.5, 38), "Cancel");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    m_Scene->addItem(cancelButton);
}

SaveDialog::~SaveDialog()
{
    delete ui;
}

void SaveDialog::saveButtonClicked()
{
    QStringList returnList;
    returnList.append(m_FileNameLineEdit->text());
    returnList.append(m_DescriptionTextEdit->toPlainText());
    emit this->saveDetailsReady(returnList);
    this->close();
}

void SaveDialog::cancelButtonClicked()
{
    this->close();
}

