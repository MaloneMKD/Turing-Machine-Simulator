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

#include "turingmachinewindow.h"
#include "ui_turingmachinewindow.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QScrollBar>
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QColorDialog>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include "popupmessagebox.h"
#include "pixmapbutton.h"
#include "savedialog.h"

TuringMachineWindow::TuringMachineWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachineWindow)
{
    ui->setupUi(this);

    //Load saved settings:
    this->loadSettings();
    ui->tapeLengthSpinBox->setValue(m_TapeLength);
    ui->playSpeedSpinBox->setValue(m_Speed);

    //Window configurations:
    this->setWindowIcon(QIcon(":/new/prefix1/Images and Icons/sim3.png"));
    this->setFont(QFont("Corbel Light"));
    ui->menuBar->setFont(QFont("Corbel Light", 11));
    ui->actionLoadTM->setFont(QFont("Corbel Light", 11));
    ui->actionLoadTM->setIcon(QIcon(":/new/prefix1/Images and Icons/load icon.png"));
    ui->actionSaveTM->setFont(QFont("Corbel Light", 11));
    ui->actionSaveTM->setIcon(QIcon(":/new/prefix1/Images and Icons/save icon.png"));
    ui->actionExit->setFont(QFont("Corbel Light", 11));
    ui->actionExit->setIcon(QIcon(":/new/prefix1/Images and Icons/exit icon.png"));
    ui->label->setFont(QFont("Corbel Light", 12));
    ui->clearPushButton->setFont(QFont("Corbel Light", 11));

    //Tab icons and font size:
    ui->tabWidget->setTabIcon(0, QIcon(":/new/prefix1/Images and Icons/design icon.png"));
    ui->tabWidget->setTabIcon(1, QIcon(":/new/prefix1/Images and Icons/summary icon.png"));
    ui->tabWidget->setTabIcon(2, QIcon(":/new/prefix1/Images and Icons/options icon.png"));
    ui->tabWidget->setTabIcon(3, QIcon(":/new/prefix1/Images and Icons/help icon.png"));
    ui->tabWidget->setFont(QFont("Corbel Light", 10));

    //Initialize variables:
    m_HasHALTState = false;
    m_HasSTARTState = false;
    m_SettingsChanged = false;
    m_Modified = false;
    m_FileLoaded = false;
    m_TMModel = nullptr;
    m_Processor = new TMProcessor(this);
    if(m_SavePath == "")
        m_SavePath = QDir::homePath() + "/Documents/Saved TMs";
    m_LoadedFile = "";
    m_LoadedDescription = "";
    m_CellWidth = 32;

    //Setup the TM scene:
    m_Scene = new TMSScene(this);
    ui->graphicsView->setScene(m_Scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setFrameStyle(QFrame::StyledPanel);
    ui->graphicsView->setFrameShadow(QFrame::Raised);
    ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->graphicsView->setFocus();
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Setup variables:
    m_NumOfStates = 0;

    connect(ui->addStateButton, SIGNAL(clicked(bool)), this, SLOT(addStateButtonClicked()));
    connect(ui->deleteStateButton, SIGNAL(clicked()), this, SLOT(deleteStateButtonClicked()));
    connect(ui->setAsStartButton, SIGNAL(clicked()), this, SLOT(setSTARTStateButtonClicked()));
    connect(ui->setAsHaltButton, SIGNAL(clicked()), this, SLOT(setHALTStateButtonClicked()));
    connect(ui->addArrowButton, SIGNAL(clicked()), this, SLOT(addArrowButtonClicked()));
    connect(ui->addLoopArrowButton, SIGNAL(clicked()), this, SLOT(addLoopArrowButtonClicked()));
    connect(ui->deleteArrowButton, SIGNAL(clicked()), this, SLOT(deleteArrowButtonClicked()));
    connect(ui->testInputButton, SIGNAL(clicked()), this, SLOT(testInputButtonClicked()));
    connect(ui->buildButton, SIGNAL(clicked()), this, SLOT(buildButtonClicked()));
    connect(ui->clearSceneButton, SIGNAL(clicked()), this, SLOT(clearSceneButtonClicked()));
    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(exitButtonClicked()));

    //Input line edit:
    ui->inputLineEdit->setMaximumWidth(2000);
    ui->inputLineEdit->setMinimumWidth(500);

    //Set up spawn box:
    m_SpawnBox = new SquareSpawnBox(5, 5, 60, 60, nullptr);
    m_Scene->addItem(m_SpawnBox);
    QGraphicsTextItem *boxLabel =  m_Scene->addText("Spawn box");
    boxLabel->setDefaultTextColor(Qt::darkGray);
    boxLabel->setPos(0, m_SpawnBox->sceneBoundingRect().height());
    boxLabel->setOpacity(0.8);

    //Setup the tape scene:
    m_TapeScene = new QGraphicsScene(this);

    //Set up tape view:
    ui->tapeGraphicsView->setMinimumWidth(500);
    ui->tapeGraphicsView->setMaximumHeight(80);
    ui->tapeGraphicsView->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    ui->tapeGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->tapeGraphicsView->setScene(m_TapeScene);
    ui->tapeGraphicsView->horizontalScrollBar()->setValue(ui->tapeGraphicsView->horizontalScrollBar()->minimum());

    //Set up the tape:
    qreal offset = 0;
    for(int i = 0; i < m_TapeLength; i++)
    {
        SquareTapeCell *temp = new SquareTapeCell(QRectF(5 + offset, 16, m_CellWidth, m_CellWidth));
        m_Tape.append(temp);
        temp->setLabel("-");
        m_TapeScene->addItem(temp);
        offset += m_CellWidth;
    }

    //Set up tape head:
    m_TapeHead = new TapeHead(this);
    m_TapeScene->addItem(m_TapeHead);
    m_TapeHeadStartXPos = 5 + (m_CellWidth / 2.0) - m_TapeHead->sceneBoundingRect().width()/2.0;
    m_TapeHead->setPos(m_TapeHeadStartXPos, 1);

    //Set up timers:
    m_MoveLeftTimer = new QTimer(this);
    m_MoveLeftTimer->setInterval(10);
    connect(m_MoveLeftTimer, SIGNAL(timeout()), this, SLOT(moveLeft()));

    m_MoveRightTimer = new QTimer(this);
    m_MoveRightTimer->setInterval(10);
    connect(m_MoveRightTimer, SIGNAL(timeout()), this, SLOT(moveRight()));

    m_PauseTimer = new QTimer(this);
    m_PauseTimer->setInterval(500);
    connect(m_PauseTimer, SIGNAL(timeout()), this, SLOT(unpause()));
    connect(this, SIGNAL(doneMoving()), this, SLOT(play()));

    //Summary page:
    this->setupSummaryPage();

        //Description timer:
    m_DescTimer = new QTimer(this);
    m_DescTimer->setInterval(500);
    connect(m_DescTimer, SIGNAL(timeout()), this, SLOT(fadeOutDescription()));

    //Options page:
    this->setupOptionsPage();

    //Help page:
    this->setupHelpPage();

    //Create save file if there isn't one
    QDir tempDir(QDir::homePath() + "/Documents");
    tempDir.mkdir("Saved TMs");

    //Show logo:
    m_Logo = m_Scene->addPixmap(QPixmap(":/new/prefix1/Images and Icons/sim3.png"));
    m_Logo->setScale(0.8);
    m_Logo->setOpacity(0.3);
    m_Logo->setPos(694 - m_Logo->sceneBoundingRect().width()/2.0, 299 - m_Logo->sceneBoundingRect().height()/2.0);
    m_LogoTimer = new QTimer(this);
    m_LogoTimer->setInterval(100);
    connect(m_LogoTimer, SIGNAL(timeout()), this, SLOT(fadeOutLogo()));
    m_TimeCounter = 0;
    m_LogoTimer->start();
}

TuringMachineWindow::~TuringMachineWindow()
{
    if(m_SettingsChanged)
    {
        //Save the new Settings:
        QFile saveFile("TMS.settings");
        if(saveFile.open(QIODevice::WriteOnly))
        {
            QTextStream outStream(&saveFile);
            outStream << m_SavePath << '\n';
            outStream << m_TapeLength << '\n';
            outStream << m_AHCColor.name() << '\n';
            outStream << m_CSCColor.name() << '\n';
            saveFile.close();
        }
    }

    delete ui;
}

void TuringMachineWindow::clearTapeView()
{
    //Move the tape head back to the start:
    m_TapeHead->setPos(m_TapeHeadStartXPos, 1);

    //Replace all letter in the tape with blanks:
    for(int i = 0; i < m_TapeLength; i++)
        m_Tape[i]->setLabel("-");

    ui->tapeGraphicsView->horizontalScrollBar()->setValue(ui->tapeGraphicsView->horizontalScrollBar()->minimum());
    m_TapeScene->update();
    ui->tapeGraphicsView->update();
}

void TuringMachineWindow::setupSummaryPage()
{
    //Summary table view:
    ui->summaryTable->setFrameStyle(QFrame::Raised);
    ui->summaryTable->setFont(QFont("Corbel", 11));
    ui->summaryTable->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_TableModel = new QStandardItemModel(this);

    //Create the table labels:
    QStringList headerLabels;
    headerLabels.append("From");
    headerLabels.append("To");
    headerLabels.append("Read");
    headerLabels.append("Write");
    headerLabels.append("Move");
    m_TableModel->setHorizontalHeaderLabels(headerLabels);
    ui->summaryTable->setModel(m_TableModel);

}

void TuringMachineWindow::setupOptionsPage()
{
    // Connect Buttons
    ui->saveLocationLineEdit->setText(m_SavePath);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(getSaveFileLocation()));
    connect(ui->playSpeedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(speedSpinBoxValueChanged(int)));
    connect(ui->tapeLengthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(tapeLengthSpinBoxValueChanged(int)));
    connect(ui->AHCButton, SIGNAL(clicked()), this, SLOT(changeAHCColor()));
    connect(ui->CSCButton, SIGNAL(clicked()), this, SLOT(changeCSCColor()));

    ui->AHCButton->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 3;}"
                                         "QPushButton:hover { border: 1px solid #ffffff;}"
                                         "QPushButton:pressed { border: 2px solid #ffffff; }").arg(m_AHCColor.name()));
    ui->CSCButton->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 3;}"
                                         "QPushButton:hover { border: 1px solid #ffffff;}"
                                         "QPushButton:pressed { border: 2px solid #ffffff; }").arg(m_CSCColor.name()));
    ui->AHC_Label->setText(m_AHCColor.name());
    ui->CSC_Label->setText(m_CSCColor.name());
}

void TuringMachineWindow::setupHelpPage()
{
    QString licenseMessage = "";
    QString licenseMessage2 = "";
    //Get license data from the files:
        //GNU License:
    /*QFile file1("COPYING.txt");
    if(file1.open(QIODevice::ReadOnly))
    {
        QTextStream instream(&file1);
        licenseMessage.append(instream.readAll());
        file1.close();
    }*/
        //LGPL License:
    QFile file2("COPYING.LESSER");
    if(file2.open(QIODevice::ReadOnly))
    {
        QTextStream instream(&file2);
        licenseMessage2.append(instream.readAll());
        file2.close();
    }

    //Page configurations:
    connect(ui->aboutAppButton, SIGNAL(clicked()), this, SLOT(aboutAppButtonClicked()));
    connect(ui->aboutQtButton, SIGNAL(clicked()), this, SLOT(aboutQtButtonClicked()));
    connect(ui->aboutAuthorButton, SIGNAL(clicked()), this, SLOT(aboutAuthorButtonClicked()));
    connect(ui->openUserManualButton, SIGNAL(clicked()), this, SLOT(openUserManualButtonClicked()));
    //ui->license1TextEdit->setPlainText(licenseMessage);
    ui->license2TextEdit->setPlainText(licenseMessage2);
}

void TuringMachineWindow::populateSummaryTable(QStringList tableData)
{
    m_TableModel->setRowCount(0);

    //Add data to the model:
    QList<QStandardItem*> row;
    for(int i = 0; i < tableData.length(); i++)
    {
        row.clear();
        row.append(new QStandardItem(QString("%1%2") .arg(tableData[i][0]) .arg(tableData[i][1])));
        row.append(new QStandardItem(QString("%1%2") .arg(tableData[i][3]) .arg(tableData[i][4])));
        row.append(new QStandardItem(tableData[i][6]));
        row.append(new QStandardItem(tableData[i][8]));
        row.append(new QStandardItem(tableData[i][10]));
        m_TableModel->appendRow(row);
        ui->summaryTable->setRowHeight(i, 35);
    }
}

void TuringMachineWindow::displayTestSummary()
{
    //Display the crash messege if any:
    QString crashMessege = m_Processor->getCrashString();
    if(crashMessege == "")
        ui->crashMessageLabel->setText("Input was accepted. No crash message.");
    else
        ui->crashMessageLabel->setText(crashMessege);

    //Set accept or reject color on box:
    if(crashMessege == "")
    {
        ui->acceptedButton->setStyleSheet("QPushButton {"
                                          "color: rgb(255, 255, 255);"
                                          "background-color: #007016;"
                                          "}"
                                          );
        ui->rejectedButton->setStyleSheet("QPushButton {"
                                          "color: rgb(255, 255, 255);"
                                          "background-color: rgb(61, 61, 61);"
                                          "}"
                                          );
    }
    else
    {
        ui->acceptedButton->setStyleSheet("QPushButton {"
                                          "color: rgb(255, 255, 255);"
                                          "background-color: rgb(61, 61, 61);"
                                          "}"
                                          );
        ui->rejectedButton->setStyleSheet("QPushButton {"
                                          "color: rgb(255, 255, 255);"
                                          "background-color: #7f0000;"
                                          "}"
                                          );
    }

    //Retrieve and display the tape and transition records:
    QStringList transitionRecord = m_Processor->getTransitionRecord().split('\n', Qt::SkipEmptyParts);
    QStringList tapeRecord = m_Processor->getTapeRecord();

    if(!transitionRecord.isEmpty() && !tapeRecord.isEmpty())
    {
        //Declare variables:
        QStringList recordDetail;
        QStringList transitionEntry;
        QFont inpTextFont;
        int index = 0;

        //Set up the font:
        inpTextFont.setFamily("Corbel");
        inpTextFont.setLetterSpacing(QFont::AbsoluteSpacing, 5);

        QString summaryText = "";
        QString transitionText = "";

        //Display all test result data:
        for(int i = 0; i< tapeRecord.length(); i++)
        {
            if(i < tapeRecord.length() - 1)
            {
                // Add Tape data to the transition text
                recordDetail = tapeRecord[i].split('-', Qt::SkipEmptyParts);
                index = recordDetail[1].toInt();

                //Add Transition data to the transition text
                transitionEntry = transitionRecord[i].split(',', Qt::SkipEmptyParts);
                transitionText += transitionEntry[0] + " ";
                transitionText += transitionEntry[1] + " ";
                transitionText += transitionEntry[2] + " ";
                transitionText += transitionEntry[3] + " ";
                transitionText += transitionEntry[4];

                // HTML tags to highlight the character on the tape in focus
                recordDetail[0].insert(index, "<b><u>");
                recordDetail[0].insert(index + 7, "</u></b>");
                summaryText += "<p>" + recordDetail[0] + "---------------------" + transitionText + "</p>";
                transitionText = "";
           }
        }
        this->ui->textEdit->setHtml(summaryText);
    }
}

void TuringMachineWindow::loadSettings()
{
    //Load Settings:
    QFile loadFile("TMS.settings");
    if(loadFile.open(QIODevice::ReadOnly))
    {
        QTextStream inStream(&loadFile);
        m_SavePath = inStream.readLine();
        QString tl = inStream.readLine();
        m_TapeLength = tl.toInt();
        m_Speed = 1;
        m_AHCColor = QColor(inStream.readLine());
        m_CSCColor = QColor(inStream.readLine());
        loadFile.close();
    }
    else
    {
        QDir dir(QDir::homePath() + "/Documents");
        dir.mkdir("Saved TMs");
        m_SavePath = dir.path() + "/Saved TMs";
        m_TapeLength = 50;
        m_Speed = 1;
        m_AHCColor = QColor("#55aa00");
        m_CSCColor = QColor("#55ffff");
    }
}

void TuringMachineWindow::quitApp()
{
    if(m_Modified)
    {
        QString message = "Your design has unsaved changes. If you do not save, the changes will be lost";
        int choice =  QMessageBox::warning(this, "Changes unsaved", message, QMessageBox::Close, QMessageBox::Cancel);
        if(choice == QMessageBox::Close)
            qApp->quit();
    }
    else
        qApp->quit();
}

void TuringMachineWindow::exitButtonClicked()
{
    this->quitApp();
}

void TuringMachineWindow::addArrowButtonClicked()
{
    QList<QGraphicsItem *> list = m_Scene->selectedItems();
    if(!list.isEmpty())
    {
        MyStateItem *temp = qgraphicsitem_cast<MyStateItem*>(list.first());
        if(temp)
            temp->createStraightArrow();
        //Signal changes as unsaved:
        if(!m_Modified)
            m_Modified = true;
    }
}

void TuringMachineWindow::addLoopArrowButtonClicked()
{
    QList<QGraphicsItem *> list = m_Scene->selectedItems();
    if(!list.isEmpty())
    {
        MyStateItem *temp = qgraphicsitem_cast<MyStateItem*>(list.first());
        if(temp)
            temp->createLoopArrow();
        //Signal changes as unsaved:
        if(!m_Modified)
            m_Modified = true;
    }
}

void TuringMachineWindow::addStateButtonClicked()
{
    QString label = QString("q%1") .arg(m_NumOfStates);
    MyStateItem *state = new MyStateItem(this, 0, label);
    state->setConnectedArrowColor(m_AHCColor);
    m_Scene->addItem(state);
    m_TM.append(state);
    m_NumOfStates++;
    //Signal changes as unsaved:
    if(!m_Modified)
        m_Modified = true;
}

void TuringMachineWindow::deleteStateButtonClicked()
{
    QList<QGraphicsItem *> list = m_Scene->selectedItems();
    QString stateNum = "";
    if(!list.isEmpty())
    {
        //Find the first state pointed to and mark it for deletion:
        MyStateItem *toBeDeleted = nullptr;
        for(int i = 0; i < list.length(); i++)
        {
            toBeDeleted = qgraphicsitem_cast<MyStateItem*>(list[i]);
            if(toBeDeleted != nullptr)
                break;
        }

        //Look of the marked state in the list of states and delete it;
        for(int i = 0; i < m_TM.length(); i++)
        {
            if(m_TM[i] == toBeDeleted)
            {
                if(i > 9)
                    stateNum = QString("%1%2") .arg(m_TM[i]->getStateName()[1]) .arg(m_TM[i]->getStateName()[2]);
                else
                    stateNum = m_TM[i]->getStateName()[1];

                //Detatch from arrows pointing at this state:
                m_TM[i]->detachFromPointedArrows();

                //Detatch this state's arrows from other states:
                m_TM[i]->detachMyArrowsFromOtherStates();

                //Check if the state being deleted is a halt state:
                if(m_TM[i]->isSTARTState())
                    m_HasSTARTState = false;

                if(m_TM[i]->isHALTState())
                    m_HasHALTState = false;

                //Rename the states:
                for(int i = stateNum.toInt(); i < m_TM.length(); i++)
                    m_TM[i]->decrementStateNameNum(i);

                //Delete the state:
                delete m_TM[i];
                m_TM.removeAt(i);

                //Signal changes as unsaved:
                if(!m_Modified)
                    m_Modified = true;
                break;
            }
        }
        m_NumOfStates--;
    }
}

void TuringMachineWindow::deleteArrowButtonClicked()
{
    QList<QGraphicsItem *> list = m_Scene->selectedItems();
    if(!list.isEmpty())
    {
        char type = ' ';
        //Check if the arrow to be deleted is a loop arrow:
        LoopArrow *tempLoop = nullptr;
        SolidArrow *temp = nullptr;
        for(int i = 0; i < list.length(); i++)//Find the first selected arrow:
        {
            //Check if the arrow to be deleted is a loop arrow:
            tempLoop = qgraphicsitem_cast<LoopArrow*>(list[i]);
            if(tempLoop)
            {
                type = 'l';
                break;
            }

            //Check if it is a solid arrow
            temp = qgraphicsitem_cast<SolidArrow*>(list[i]);
            if(temp)
            {
                type = 's';
                break;
            }
        }

        //Find the owner of the arrow and tell them to delete it:
        for(int i = 0; i <m_TM.length(); i++)
        {
            if(type == 's')
            {
                if(m_TM[i]->isAncestorOf(temp) && temp)
                {
                    m_TM[i]->deleteArrow(temp);
                    break;
                }
            }
            else if(type == 'l')
            {
                if(m_TM[i]->isAncestorOf(tempLoop) && tempLoop)
                {
                    m_TM[i]->deleteArrow(tempLoop);
                    //Signal changes as unsaved:
                    if(!m_Modified)
                        m_Modified = true;
                    break;
                }
            }
        }
    }
}

void TuringMachineWindow::buildButtonClicked()
{
    //Check if the TM has a start and halt state:
    bool startAvailable = false;
    bool haltAvailable = false;
    for(MyStateItem *s : m_TM)
    {
        if(s->isSTARTState())
            startAvailable = true;
        if(s->isHALTState())
            haltAvailable = true;
    }

    if(m_TM.isEmpty())//If TM is empty:
    {
        PopUpMessagebox *noTMFound = new PopUpMessagebox(this, "No TM Detected", "Please design a TM before trying to buid one."
                                                                       "\n\n\nHints:"
                                                                       "\n- Make sure that every arrow is connected to a state.",
                                               QPixmap(":/new/prefix1/Images and Icons/warning.png"));
        noTMFound->show();
    }
    else if(!startAvailable)
    {
        QString message = "No START state was detected in your TM. Please add one then build again.";
        PopUpMessagebox *noStart = new PopUpMessagebox(this, "No START state detected", message,
                                                       QPixmap(":/new/prefix1/Images and Icons/warning.png"));
        noStart->show();
    }
    else if(!haltAvailable)
    {
        QString message = "No HALT state was detected in your TM. Please add one then build again.";
        PopUpMessagebox *noHalt = new PopUpMessagebox(this, "No HALT state detected", message,
                                                       QPixmap(":/new/prefix1/Images and Icons/warning.png"));
        noHalt->show();
    }
    else
    {
        int badStateIndex = -1;
        MyStateItem::Status s;

        //Check if all the arrows are pointing to a state and have proper labels:
        for(int i = 0; i < m_TM.length(); i++)
        {
            s = m_TM[i]->readyForProcessing();
            if(s != MyStateItem::Ready || (!m_TM[i]->hasArrows() && !m_TM[i]->isHALTState()))
            {
                badStateIndex = i;
                break;
            }
        }

        if(badStateIndex > -1)//If there is a state not ready:
        {
            QString message;
            if(s == MyStateItem::ArrowNotPointingToState)
            {
                message = QString("The state %1 is not ready for processing."
                                          "\n\nPlease make sure that the state has edges and that all edges point to a state.")
                              .arg(m_TM[badStateIndex]->getStateName());
            }
            else if(s == MyStateItem::ArrowLabelInvalid)
            {
                message = QString("The state %1 is not ready for processing."
                                  "\n\nOne or more of the arrows has an invalid label. Please make sure the labels are of the form:\n\n"
                                  "(Read Letter),(Write Letter), (Move direction letter)\n\nIf there is more than one label on an arrow,"
                                  "please enter these on separate lines.")
                              .arg(m_TM[badStateIndex]->getStateName());
            }
            PopUpMessagebox *notReady = new PopUpMessagebox(this, "TM not ready for processing", message, QPixmap(":/new/prefix1/Images and Icons/warning.png"));
            notReady->show();
        }
        else//If all states are ready:
        {
            QStringList statesData;
            QString entry = "";
            for(int i = 0; i < m_TM.length(); i++)
            {
                entry = m_TM[i]->getStateData();
                statesData.append(entry);
            }

            //Give the data to the machine model:
            if(m_TMModel != nullptr)
                delete m_TMModel;
            m_TMModel = new TuringMachine(statesData);
            m_TMModel->build();

            //Update the summary table:
            this->populateSummaryTable(m_TMModel->getSummaryTableData());

            //Inform the user that the machine built successfully:
            PopUpMessagebox *success = new PopUpMessagebox(this, "TM built successfully", "Your TM was built successfully."
                                                                                          "\n\nYou may now begin testing input.",
                                                           QPixmap(":/new/prefix1/Images and Icons/hammer.png"));
            success->show();
        }
    }
}

void TuringMachineWindow::setHALTStateButtonClicked()
{
    if(!m_HasHALTState)
    {
        QList<QGraphicsItem *> list = m_Scene->selectedItems();
        if(!list.isEmpty())
        {
            MyStateItem* temp = nullptr;
            for(int i = 0; i < list.length(); i++)
            {
                temp = qgraphicsitem_cast<MyStateItem*>(list[i]);
                if(temp && !temp->isHALTState() && !temp->isSTARTState())
                {
                    temp->setIsHALTState();
                    m_HasHALTState = true;

                    //Signal changes as unsaved:
                    if(!m_Modified)
                        m_Modified = true;
                    break;
                }
            }
        }
    }
}

void TuringMachineWindow::setSTARTStateButtonClicked()
{
    if(!m_HasSTARTState)
    {
        QList<QGraphicsItem *> list = m_Scene->selectedItems();
        if(!list.isEmpty())
        {
            MyStateItem* temp = nullptr;
            for(int i = 0; i < list.length(); i++)
            {
                temp = qgraphicsitem_cast<MyStateItem*>(list[i]);
                if(temp && !temp->isHALTState() && !temp->isSTARTState())
                {
                    temp->setIsSTARTState();
                    m_HasSTARTState = true;
                    //Signal changes as unsaved:
                    if(!m_Modified)
                        m_Modified = true;
                    break;
                }
            }
        }
    }
}

void TuringMachineWindow::testInputButtonClicked()
{
    if(m_TMModel == nullptr)
    {
        QString message = "No TM detected. \n\nPlease click the \"Build\" button after designing your TM to build it before testing input.";
        PopUpMessagebox *noTMDetected = new PopUpMessagebox(this, "No TM Detected", message, QPixmap(":/new/prefix1/Images and Icons/warning.png"));
        noTMDetected->show();
    }
    else
    {
        // Stop the timers if they are running
        if(m_MoveLeftTimer->isActive())
            m_MoveLeftTimer->stop();
        if(m_MoveRightTimer->isActive())
            m_MoveRightTimer->stop();
        if(m_PauseTimer->isActive())
            m_PauseTimer->stop();

        // Disable the lear button
         ui->clearPushButton->setEnabled(false);

        // Set the input edit as read-only
        ui->inputLineEdit->setReadOnly(true);

        // Disable tape length spinbox
        ui->tapeLengthSpinBox->setReadOnly(true);

        //Reset TM colors:
        for(MyStateItem *s: m_TM)
            s->changeColor(Qt::white);

        //Reset the tape:
        this->on_inputLineEdit_editingFinished();

        //Get the input string, set parameters and test the string:
        QString input = ui->inputLineEdit->text() + '-';
        m_Processor->setParameters(input, m_TMModel);
        TMProcessor::ProcessResult result = m_Processor->start();

        if(result == TMProcessor::Successful)
        {
            //Play the hops:
            m_MachineData = m_Processor->getMachineData();
            m_TapeData = m_Processor->getTapeData();

            m_CurrentCell = 0;
            m_TapeCounter = 0;
            m_Count = 0;

            m_TapeHeadStartXPos = 5 + (m_CellWidth / 2.0) - m_TapeHead->sceneBoundingRect().width()/2.0;
            this->play();

            //Display the test summary:
            this->displayTestSummary();
        }
        else
        {
            QString message = "There is a possible infinite loop in your TM.\n"
                              "1 000 000 read, write and move iterations where executed.\n"
                              "Please revise your TM design so as to remove the infinite loop.\n\n"
                              "Hint\n- See the help section for help identifying infinite loops.";

            PopUpMessagebox *infLoopMessage = new PopUpMessagebox(this, "Infinite loop warning", message,
                                                                 QPixmap(":/new/prefix1/Images and Icons/warning.png"));
            infLoopMessage->show();
        }
    }
}

void TuringMachineWindow::clearSceneButtonClicked()
{
    for(int i = 0; i < m_TM.length(); i++)
    {
        m_Scene->removeItem(m_TM[i]);
        delete m_TM[i];
    }
    m_NumOfStates = 0;
    m_TM.clear();
    m_TableModel->setRowCount(0);
    m_FileLoaded = false;
    m_LoadedFile = "";
    m_LoadedDescription = "";

    //Reset the summary page details:
    ui->acceptedButton->setStyleSheet("QPushButton {"
                                        "color: rgb(255, 255, 255);"
                                        "background-color: rgb(61, 61, 61);"
                                    "}");
    ui->rejectedButton->setStyleSheet("QPushButton {"
                                      "color: rgb(255, 255, 255);"
                                      "background-color: rgb(61, 61, 61);"
                                      "}");

    m_NumOfStates = 0;
    ui->crashMessageLabel->setText("Reason for crash will appear here");
    ui->textEdit->clear();
}

void TuringMachineWindow::on_inputLineEdit_editingFinished()
{
    //Clear the tape view:
    this->clearTapeView();

    //Enter the string in the tape:
    if(!m_Tape.isEmpty())
    {
        QString inputString = ui->inputLineEdit->text();
        for(int i = 0; i < inputString.length(); i++)
        {
            //In case the input string is longer than the tape:
            if(i == m_TapeLength - 1)
                break;
            m_Tape[i]->setLabel(inputString[i]);
        }
    }
}

void TuringMachineWindow::play()
{
    if(m_Count < m_MachineData.length() - 1)
    {
        //Change the previous state's color to white:
        if(m_Count > 0)
            m_TM[m_MachineData[m_Count]]->changeColor(Qt::white);

        //Change the current state's color:
        if(m_MachineData.length() > m_Count + 1)
            m_TM[m_MachineData[m_Count + 1]]->changeColor(m_CSCColor);

        //Move tapehead:
        this->moveTapeHead();
        m_Count++;
    }
    else
    {
        //Change the last state that the input got us to green if the word was accepted otherwise red:
        if(m_Processor->getCrashString() == "")
            m_TM[m_MachineData[m_Count]]->changeColor(Qt::green);
        else
            m_TM[m_MachineData[m_Count]]->changeColor(Qt::red);

        // Re-enable everything that was disabled
        ui->clearPushButton->setEnabled(true);
        ui->inputLineEdit->setReadOnly(false);
        ui->tapeLengthSpinBox->setReadOnly(false);
    }
}

void TuringMachineWindow::unpause()
{
    m_TapeHead->setBrush(Qt::black);
    m_PauseTimer->stop();
    emit this->doneMoving();
}

void TuringMachineWindow::moveTapeHead()
{
    //Move tape head:
    if(m_TapeCounter < m_TapeData.length() - 1)
    {
        m_MoveCounter = 0;
        m_PrevHeadPos = m_TapeHead->scenePos();

        //Write letter on tape:
        m_Tape[m_CurrentCell]->setLabel(m_TapeData[m_TapeCounter][0]);

        if(m_TapeData[m_TapeCounter][1].toLower() == QString("l"))
        {
            m_MoveLeftTimer->start();
            m_CurrentCell--;

        }
        else if(m_TapeData[m_TapeCounter][1].toLower() == QString("r"))
        {
            m_MoveRightTimer->start();
            m_CurrentCell++;
        }
    }
}

void TuringMachineWindow::moveLeft()
{
    if(m_MoveCounter < m_CellWidth)
    {
        m_TapeHead->setPos(m_TapeHead->pos().x() - 1, m_TapeHead->scenePos().y());
        m_MoveCounter++;
    }
    else
    {
        m_MoveLeftTimer->stop();
        m_TapeCounter++;
        m_TapeHead->setBrush(Qt::white);
        m_PauseTimer->start();
    }
}

void TuringMachineWindow::moveRight()
{
    if(m_MoveCounter < m_CellWidth)
    {
        m_TapeHead->setPos(m_TapeHead->scenePos().x() + 1, m_TapeHead->scenePos().y());
        m_MoveCounter++;
    }
    else
    {
        m_MoveRightTimer->stop();
        m_TapeCounter++;
        m_TapeHead->setBrush(Qt::white);
        m_PauseTimer->start();
    }
}

void TuringMachineWindow::on_clearPushButton_clicked()
{
    ui->inputLineEdit->clear();
    this->on_inputLineEdit_editingFinished();
}


void TuringMachineWindow::on_actionSaveTM_triggered()
{
    auto saveDialog = [this](){
        SaveDialog *sv = new SaveDialog(this);
        connect(sv, SIGNAL(saveDetailsReady(QStringList)), this, SLOT(getSaveDetails(QStringList)));
        sv->show();
    };

    if(m_FileLoaded)
    {
        QStringList datList = m_LoadedFile.split('/');
        m_LoadedFile = datList[datList.length() - 1];
        QString message = QString("Save over the loaded file: %1?") .arg(m_LoadedFile);
        int choice = QMessageBox::question(this, "Override File?", message, QMessageBox::Yes, QMessageBox::No);
        if(choice == QMessageBox::Yes)
        {
            //Save over currently open file
            QStringList tempList;
            m_LoadedFile.remove(".xml");
            tempList.append(m_LoadedFile);
            tempList.append(m_LoadedDescription);
            this->getSaveDetails(tempList);
        }
        else
            saveDialog();
    }
    else
        saveDialog();
}

void TuringMachineWindow::on_actionLoadTM_triggered()
{
    QString loadPath = QDir::homePath() + "/Documents/Saved TMs";
    QString loadFileName = QFileDialog::getOpenFileName(this, "Open TM", loadPath, "XML files (*.xml)");
    m_LoadedFile = loadFileName;

    //Temporary objects for loading purposes:
    MyStateItem *tempState = nullptr;
    LoopArrow *tempLoopArrow = nullptr;
    SolidArrow *tempSolidArrow = nullptr;

    QString description;

    if(loadFileName != QString(""))
    {
        QFile loadFile(loadFileName);
        if(loadFile.open(QFile::ReadOnly))
        {
            //Clear the TM to prepare to load:
            for(int i = 0; i < m_TM.length(); i++)
                delete m_TM[i];
            m_TM.clear();

            //Open load file and load TM:
            QXmlStreamReader reader(&loadFile);

            //Variable declaration:
            int numOfEdges;
            bool isStartState;
            bool isHaltState;
            bool firstLoop = true;
            QPointF scenePos ;
            QString name = "";

            //Get the TM description:
            reader.readNext();
            while(!reader.atEnd())
            {
                numOfEdges = 0;
                name = "";
                isStartState = false;
                isHaltState = false;

                reader.readNext();
                if(firstLoop)
                {
                    description = reader.attributes().value("Description").toString();
                    firstLoop = false;
                }

                if(reader.name() == QString("State") && reader.isStartElement())
                {
                    //State name:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("Name"))
                        name = reader.readElementText();

                    //Start state:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("StartState"))
                    {
                        if(reader.readElementText() == QString("true"))
                            isStartState = true;
                        else if(reader.readElementText() == QString("false"))
                            isStartState = false;
                    }

                    //Halt state:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("HaltState"))
                    {
                        if(reader.readElementText() == QString("true"))
                            isHaltState = true;
                        else if(reader.readElementText() == QString("false"))
                            isHaltState = false;
                    }

                    //Scene position:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("ScenePosition"))
                    {
                        QString points = reader.readElementText();
                        QStringList sl = points.split(',');
                        scenePos.setX(sl[0].toDouble());
                        scenePos.setY(sl[1].toDouble());
                    }

                    //Create the state:
                    tempState = new MyStateItem(this,0, name);
                    if(isStartState)
                        tempState->setIsSTARTState();
                    if(isHaltState)
                        tempState->setIsHALTState();
                    tempState->setPos(scenePos);

                    //Number of edges:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("Edges"))
                        numOfEdges = reader.attributes().value("NumberOfEdges").toInt();

                    //Check for loop arrow:
                    reader.readNext();//Close
                    reader.readNext();
                    if(reader.name() == QString("LoopEdge"))
                    {
                        reader.readNext();//Close
                        reader.readNext();
                        QString loopLabel = "";
                        if(reader.name() == QString("Label"))
                        {
                            loopLabel = reader.readElementText();
                            loopLabel.replace('/', '\n');
                        }
                        reader.readNext();//Close
                        reader.readNext();
                        QString lengths = "";
                        QList<qreal> numLengths;
                        if(reader.name() == QString("LineLengths"))
                        {
                            lengths = reader.readElementText();
                            QStringList stringNums = lengths.split(',', Qt::SkipEmptyParts);
                            for(int i = 0; i < stringNums.length(); i++)
                                numLengths.append(stringNums[i].toDouble());
                        }
                        reader.readNext();//Close
                        reader.readNext();
                        QString pointingTo = "";
                        if(reader.name() == QString("PointingTo"))
                            pointingTo = reader.readElementText();
                        reader.readNext();//Close
                        reader.readNext();
                        qreal loopRotation = 0;
                        if(reader.name() == QString("Rotation"))
                            loopRotation = reader.readElementText().toDouble();

                        tempLoopArrow = new LoopArrow(tempState, tempState);
                        tempLoopArrow->setLabel(loopLabel);
                        tempLoopArrow->setLineLengths(numLengths);
                        tempLoopArrow->correctLabelHeight();
                        tempState->createLoopArrow(tempLoopArrow, loopRotation);
                        reader.readNext();//Close
                        reader.readNext();
                        reader.readNext();
                        reader.readNext();
                        numOfEdges--;
                    }

                    //Check for other arrows:
                    QString solidArrowLabel = "";
                    QString pointingTo = "";
                    QString lengths = "";
                    QList<qreal> numLengths;
                    qreal length = 0;
                    qreal rot = 0;
                    bool bent = false;
                    for(int i = 0; i < numOfEdges; i++)
                    {
                        //Gather the data:
                        if(reader.name().contains(QString("Edge")))
                        {
                            //Bent status:
                            if(reader.attributes().value("Bent").toString() == "True")
                                bent = true;

                            //Line lengths:
                            lengths = reader.attributes().value("LineLengths").toString();
                            QStringList stringNums = lengths.split(',', Qt::SkipEmptyParts);
                            for(int j = 0; j < stringNums.length(); j++)
                                numLengths.append(stringNums[j].toDouble());

                            reader.readNext();//Close
                            reader.readNext();
                            if(reader.name() == QString("Label"))
                            {
                                solidArrowLabel = reader.readElementText();
                                solidArrowLabel.replace('/', '\n');
                            }
                            reader.readNext();//Close
                            reader.readNext();
                            if(reader.name() == QString("PointingTo"))
                                pointingTo = reader.readElementText();
                            reader.readNext();//Close
                            reader.readNext();
                            if(reader.name() == QString("Length"))
                                length = reader.readElementText().toDouble();
                            reader.readNext();//Close
                            reader.readNext();
                            if(reader.name() == QString("Rotation"))
                                rot = reader.readElementText().toDouble();

                            reader.readNext();//Close
                            reader.readNext();//Close Edge
                            reader.readNext();//Close
                            reader.readNext();//Next edge or Close Edges

                            //Create the edge:
                            tempSolidArrow = new SolidArrow(tempState, tempState);
                            tempSolidArrow->setLabel(solidArrowLabel);
                            tempSolidArrow->setStatePointed(pointingTo);
                            tempSolidArrow->setRotation(rot);
                            tempSolidArrow->lengthenBy((length - 3)*3.0);
                            tempSolidArrow->setFMAL(true);
                            tempSolidArrow->setLineLengths(numLengths);
                            if(bent == true)
                            {
                                tempSolidArrow->bend();
                                tempSolidArrow->setJustLoaded(true);
                            }

                            //Add edge to the state:
                            tempState->createStraightArrow(tempSolidArrow);
                        }
                    }
                    reader.readNext();//Close
                    reader.readNext();//Close State
                    reader.readNext();//Close
                    m_Scene->addItem(tempState);
                    m_TM.append(tempState);
                }
            }
            //Set the arrow highlight color for the states:
            for(MyStateItem *s : m_TM)
                s->setConnectedArrowColor(m_AHCColor);

            //Update the number of lengths:
            m_NumOfStates = m_TM.length();

            //If there was an error while reading the file print otherwise display description:
            if(reader.hasError())
            {
                QString message = "Error: " + reader.errorString() + "on line number: " + QString::number(reader.lineNumber());
                PopUpMessagebox *readError = new PopUpMessagebox(this, "Error reading file", message, QPixmap(":/new/prefix1/Images and Icons/warning.png"));
                readError->show();
            }
            else
            {
                //Set up the description, its title and box:
                m_DescriptionLabel = m_Scene->addText("Description:");
                QFont labelFont;
                labelFont.setFamily("Corbel Light");
                labelFont.setPointSizeF(10.0);
                labelFont.setUnderline(true);
                m_DescriptionLabel->setFont(labelFont);
                m_DescriptionLabel->setPos(1000, 5);

                m_TMDescriptionTextEdit = new QTextEdit;
                m_TMDescriptionTextEdit->setFixedSize(300, 120);
                m_TMDescriptionTextEdit->setFrameStyle(QFrame::Raised);
                m_TMDescriptionTextEdit->setText(description);
                m_TMDescriptionTextEdit->setFont(QFont("Corbel Light", 10));
                m_DescEditProxy = m_Scene->addWidget(m_TMDescriptionTextEdit);
                m_DescEditProxy->setPos(1000, 30);
                m_DescEditProxy->setZValue(-1);
                m_LoadedDescription = description;

                //Reset counter and start timer:
                m_DisplayCounter = 0;
                m_DescTimer->start();
                m_FileLoaded = true;
            }
            loadFile.close();
        }
    }
}

void TuringMachineWindow::on_actionExit_triggered()
{    
    this->quitApp();
}

void TuringMachineWindow::getSaveFileLocation()
{
    QString sfl = QFileDialog::getExistingDirectory(this, "Choose save location", m_SavePath);
    if(sfl != "")
    {
        QDir loc(sfl);
        if(loc.exists())
            m_SavePath = sfl;
        m_SaveLocationLineEdit->setText(sfl);
        m_SettingsChanged = true;
    }
}

void TuringMachineWindow::changeAHCColor()
{
    QColor newColor = QColorDialog::getColor(m_AHCColor, this, "Pick a color");
    if(newColor.isValid())
    {
        ui->AHCButton->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 3;}"
                                             "QPushButton:hover { border: 1px solid #ffffff;}"
                                             "QPushButton:pressed { border: 2px solid #ffffff; }").arg(newColor.name()));
        m_AHCColor = newColor;
        ui->AHC_Label->setText(newColor.name());

        for(MyStateItem *s : m_TM)
            s->setConnectedArrowColor(newColor);
        m_SettingsChanged = true;
    }
}

void TuringMachineWindow::changeCSCColor()
{
    QColor newColor = QColorDialog::getColor(m_CSCColor, this, "Pick a color");
    if(newColor.isValid())
    {
        ui->CSCButton->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 3;}"
                                             "QPushButton:hover { border: 1px solid #ffffff;}"
                                             "QPushButton:pressed { border: 2px solid #ffffff; }").arg(m_CSCColor.name()));
        m_CSCColor = newColor;
        ui->CSC_Label->setText(newColor.name());

        m_CSCColor = newColor;
        ui->CSC_Label->setText(m_CSCColor.name());
        m_SettingsChanged = true;
    }
}

void TuringMachineWindow::speedSpinBoxValueChanged(int val)
{
    m_Speed = val;
    if(m_Speed <= 3)
    {
        m_PauseTimer->setInterval(1/m_Speed * 500);
        m_MoveLeftTimer->setInterval(10);
        m_MoveRightTimer->setInterval(10);
    }
    else
    {
        m_MoveLeftTimer->setInterval(12 - m_Speed);
        m_MoveRightTimer->setInterval(12 - m_Speed);
    }
    m_SettingsChanged = true;
}

void TuringMachineWindow::tapeLengthSpinBoxValueChanged(int val)
{
    m_TapeLength = val;

    // Clear tape scene
    m_Tape.clear();

    //Recreate tape scene
    if(m_TapeScene)
        delete m_TapeScene;
    m_TapeScene = new QGraphicsScene(this);
    ui->tapeGraphicsView->setScene(m_TapeScene);

    //Set up tape head:
    m_TapeHead = new TapeHead(this);
    m_TapeScene->addItem(m_TapeHead);
    m_TapeHead->setPos(m_TapeHeadStartXPos, 1);

    //Set up the tape:
    qreal offset = 0;
    for(int i = 0; i < m_TapeLength; i++)
    {
        SquareTapeCell *temp = new SquareTapeCell(QRectF(5 + offset, 16, m_CellWidth, m_CellWidth));
        m_Tape.append(temp);
        temp->setLabel("-");
        m_TapeScene->addItem(temp);
        offset += m_CellWidth;
    }
    m_SettingsChanged = true;
}

void TuringMachineWindow::aboutAppButtonClicked()
{
    QString message = QString("TURING MACHINE SIMULATOR\n\n"
                              "Copyright (C) 2023 Malone Kudakwashe Napier-Jameson\n\n"
                              "This program was made to simulate a Turing Machine and was written in QtCreator using the Qt framework.\n"
                              "It is an open source project and code can be found on Git at the link in the \"About Author\" tab.\n"
                              "It allows you to design your TM and test words that you designed it to accept.\n"
                              "This program also includes checks to prevent infinite loops and provides warning if a possible infinite loop"
                              "is detected.\n"
                              "The program also provides a summary table and summary of execution of input so that you can"
                              "trace the processing of the input and many other useful features for a Comp-Sci student.\n"
                              "It also provides the option of saving and loading. TMs are saved in XML format and thus can be easily shared"
                              "with others and edited.\n"
                              "For more information, see the user manual.");
    PopUpMessagebox *appInfo = new PopUpMessagebox(this, "About app", message, QPixmap(":/new/prefix1/Images and Icons/hammer.png"));
    appInfo->show();
}

void TuringMachineWindow::aboutQtButtonClicked()
{
    qApp->aboutQt();
}

void TuringMachineWindow::aboutAuthorButtonClicked()
{
    QString message = QString("Author:\tMalone Kudakwashe Napier-Jameson\n"
                      "Email:\tMK.Napier-Jameson@outlook.com\n\n"
                      ""
                      "Comments:\nI had a great time coding this application and learning more about the Qt framework\n"
                      "at the same time. I hope this app will be a useful tool in learning and experimenting\n"
                      "in your theoretical computer science course.\n"
                      "If you have any questions or comments please contact me over email. Have fun!\n");
    PopUpMessagebox *authorInfo = new PopUpMessagebox(this, "About author", message, QPixmap(":/new/prefix1/Images and Icons/hammer.png"));
    authorInfo->show();
}

void TuringMachineWindow::getSaveDetails(QStringList details)
{
    //Get the save name and open the file:
    //Make the directory in the Documents folder if it doesnt exist:
    QDir dir;
    dir.mkdir(m_SavePath);

    QString fileName = QString(m_SavePath + "/" + details[0] + ".xml");
    QString description = details[1];

    if(fileName != "")
    {
            //Make the file:
        QFile saveFile(fileName);
        if(saveFile.open(QFile::WriteOnly))
        {
            //Create an xml writer and write to the file:
            QXmlStreamWriter writer(&saveFile);
            writer.setAutoFormatting(true);

            writer.writeStartDocument();
            writer.writeStartElement("TuringMachine");
            writer.writeAttribute("Description", description);

            int offset = 0;

            //Write each state individually:
            for(int i = 0; i < m_TM.length(); i++)
            {
                offset = 0;
                QStringList stateData = m_TM[i]->getSaveData();
                writer.writeStartElement("State");
                writer.writeTextElement("Name", stateData[0]);
                writer.writeTextElement("StartState", stateData[1]);
                writer.writeTextElement("HaltState", stateData[2]);
                writer.writeTextElement("ScenePosition", stateData[3]);
                int numEdges = stateData[4].toInt();

                writer.writeStartElement("Edges");
                writer.writeAttribute("NumberOfEdges", QString::number(numEdges));
                if(stateData[5] == "++")
                {
                    writer.writeStartElement("LoopEdge");
                    writer.writeTextElement("Label", stateData[6]);
                    writer.writeTextElement("LineLengths", stateData[7]);
                    writer.writeTextElement("PointingTo", stateData[8]);
                    writer.writeTextElement("Rotation", stateData[9]);
                    writer.writeEndElement();//Loop edge
                    numEdges--;
                    offset = 4;
                }

                int edgeOffset = 0;
                for(int j = 0; j < numEdges; j++)
                {
                    writer.writeStartElement(QString("Edge%1") .arg(j));
                    if(stateData[8 + offset + edgeOffset] == "bent")
                        writer.writeAttribute("Bent", "True");
                    else
                        writer.writeAttribute("Bent", "False");
                    writer.writeAttribute("LineLengths", stateData[7 + offset + edgeOffset]);
                    writer.writeTextElement("Label", stateData[6 + offset + edgeOffset]);
                    writer.writeTextElement("PointingTo", stateData[9 + offset + edgeOffset]);
                    writer.writeTextElement("Length", stateData[10 + offset + edgeOffset]);
                    writer.writeTextElement("Rotation", stateData[11 + offset + edgeOffset]);
                    writer.writeEndElement();//Edge %1
                    edgeOffset += 4;
                }
                writer.writeEndElement();//Edges
                writer.writeEndElement();//State
            }

            writer.writeEndElement();//Turing Machine
            writer.writeEndDocument();

            //Close the file:
            saveFile.close();
            //Signal changes as unsaved:
            if(m_Modified)
                m_Modified = false;
        }
        else
            QMessageBox::warning(this, "Error", "Failed to open the save file!");
    }
        else
        QMessageBox::warning(this, "Error", "File name is blank. File not saved!");
}

void TuringMachineWindow::fadeOutDescription()
{
    if(m_DisplayCounter >= 3)
    {
        m_DescriptionLabel->setOpacity(m_DescriptionLabel->opacity() - 0.08);
        m_DescEditProxy->setOpacity(m_DescEditProxy->opacity() - 0.08);
        if(m_DescriptionLabel->opacity() <= 0)
        {
            m_DescTimer->stop();
            m_Scene->removeItem(m_DescriptionLabel);
            m_Scene->removeItem(m_DescEditProxy);
            delete m_TMDescriptionTextEdit;
        }
    }
    else
        m_DisplayCounter += 0.5;
}

void TuringMachineWindow::openUserManualButtonClicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("TMSimulator Manual.pdf"));
}

void TuringMachineWindow::fadeOutLogo()
{

    if(m_TimeCounter >= 20)
    {
        if(m_Logo->opacity() > 0)
            m_Logo->setOpacity(m_Logo->opacity() - 0.01);
        else
        {
            m_LogoTimer->stop();
            m_Scene->removeItem(m_Logo);
        }
    }
    else
        m_TimeCounter += 1;
}

void TuringMachineWindow::resetConnections()
{
    emit this->scaleChanged();
}

