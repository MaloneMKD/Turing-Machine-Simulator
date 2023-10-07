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

    const int TM_SCENE_WIDTH = 1515, TM_SCENE_HEIGHT = 598;
    //Setup the TM scene:
    m_Scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_Scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setFrameStyle(QFrame::StyledPanel);
    ui->graphicsView->setFrameShadow(QFrame::Raised);
    ui->graphicsView->setFixedSize(TM_SCENE_WIDTH + 5, TM_SCENE_HEIGHT + 5);

    //Setup variables:
    m_NumOfStates = 0;
    m_Scene->setSceneRect(0,0,TM_SCENE_WIDTH,TM_SCENE_HEIGHT);
    m_Scene->setSceneRect(ui->graphicsView->sceneRect());

    //Setup line divide:
    m_Scene->addLine(1388, 15, 1388, 583, QPen(Qt::gray, 2.0));

    //Set up buttons:
    int buttonX = 1398;
    int buttonY = 20;

        //Add state button:
    SquareButton *addStateButton = new SquareButton(this, QRectF(buttonX, buttonY, 100, 30), "Add State");
    connect(addStateButton, SIGNAL(clicked()), this, SLOT(addStateButtonClicked()));
    m_Scene->addItem(addStateButton);
        //Delete state button:
    SquareButton *deleteStateButton = new SquareButton(this, QRectF(buttonX, buttonY + 38, 100, 30), "Delete State");
    connect(deleteStateButton, SIGNAL(clicked()), this, SLOT(deleteStateButtonClicked()));
    m_Scene->addItem(deleteStateButton);
        //Set start state button:
    SquareButton *setStartStateButton = new SquareButton(this, QRectF(buttonX, buttonY + 76, 100, 30), "Set as START State");
    connect(setStartStateButton, SIGNAL(clicked()), this, SLOT(setSTARTStateButtonClicked()));
    m_Scene->addItem(setStartStateButton);
        //Set as halt state button:
    SquareButton *setHaltStateButton = new SquareButton(this, QRectF(buttonX, buttonY + 114, 100, 30), "Set as HALT State");
    connect(setHaltStateButton, SIGNAL(clicked()), this, SLOT(setHALTStateButtonClicked()));
    m_Scene->addItem(setHaltStateButton);

    m_Scene->addRect(1393, 15, 110, 154, QPen(Qt::lightGray));
    m_Scene->addRect(1431, 12, 40, 12, QPen(Qt::white), QBrush(Qt::white));
    QGraphicsTextItem *scLabel = m_Scene->addText("State", QFont("Corbel light", 10.0));
    scLabel->setPos(1432, -1);

        //Add arrow button:
    SquareButton *addArrowButton = new SquareButton(this, QRectF(buttonX, buttonY + 192, 100, 30), "Add Arrow");
    connect(addArrowButton, SIGNAL(clicked()), this, SLOT(addArrowButtonClicked()));
    m_Scene->addItem(addArrowButton);
        //Add loop arrow button:
    SquareButton *addLoopArrowButton = new SquareButton(this, QRectF(buttonX, buttonY + 230, 100, 30), "Add Loop Arrow");
    connect(addLoopArrowButton, SIGNAL(clicked()), this, SLOT(addLoopArrowButtonClicked()));
    m_Scene->addItem(addLoopArrowButton);
        //Delete arrow button:
    SquareButton *deleteArrowButton = new SquareButton(this, QRectF(buttonX, buttonY + 268, 100, 30), "Delete Arrow");
    connect(deleteArrowButton, SIGNAL(clicked()), this, SLOT(deleteArrowButtonClicked()));
    m_Scene->addItem(deleteArrowButton);

    m_Scene->addRect(1393, 207, 110, 116, QPen(Qt::lightGray));
    m_Scene->addRect(1430, 206, 40, 12, QPen(Qt::white), QBrush(Qt::white));
    QGraphicsTextItem *acLabel = m_Scene->addText("Arrow", QFont("Corbel light", 10.0));
    acLabel->setPos(1430, 192);

        //Build button:
    SquareButton *buildButton = new SquareButton(this, QRectF(buttonX, buttonY + 344, 100, 30), "Build");
    connect(buildButton, SIGNAL(clicked()), this, SLOT(buildButtonClicked()));
    m_Scene->addItem(buildButton);
        //Test input button:
    SquareButton *testInputButton = new SquareButton(this, QRectF(buttonX, buttonY + 382, 100, 30), "Test Input");
    connect(testInputButton, SIGNAL(clicked()), this, SLOT(testInputButtonClicked()));
    m_Scene->addItem(testInputButton);

    m_Scene->addRect(1393, 359, 110, 78, QPen(Qt::lightGray));
    m_Scene->addRect(1425, 359, 46, 12, QPen(Qt::white), QBrush(Qt::white));
    QGraphicsTextItem *pcLabel = m_Scene->addText("Process", QFont("Corbel light", 10.0));
    pcLabel->setPos(1425, 344);

    //Clear scene button:
    SquareButton *clearButton = new SquareButton(this, QRectF(buttonX, buttonY + 454, 100, 30), "Clear Scene");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearSceneButtonClicked()));
    m_Scene->addItem(clearButton);

    m_Scene->addRect(1393, 469, 110, 40, QPen(Qt::lightGray));
    m_Scene->addRect(1429, 465, 35, 12, QPen(Qt::white), QBrush(Qt::white));
    QGraphicsTextItem *cscLabel = m_Scene->addText("Clear", QFont("Corbel light", 10.0));
    cscLabel->setPos(1430, 455);

    //Exit button:
    SquareButton *exitButton = new SquareButton(this, QRectF(buttonX, buttonY + 528, 100, 30), "Exit");
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exitButtonClicked()));
    m_Scene->addItem(exitButton);

    m_Scene->addRect(1393, 543, 110, 40, QPen(Qt::lightGray));
    m_Scene->addRect(1433, 538, 28, 12, QPen(Qt::white), QBrush(Qt::white));
    QGraphicsTextItem *ecLabel = m_Scene->addText("Exit", QFont("Corbel light", 10.0));
    ecLabel->setPos(1434, 528);

    //Input line edit:
    ui->inputLineEdit->setFixedWidth(1400);

    //Set up spawn box:
    m_SpawnBox = new SquareSpawnBox(5, 5, 60, 60, nullptr);
    m_Scene->addItem(m_SpawnBox);
    QGraphicsTextItem *boxLabel =  m_Scene->addText("Spawn box");
    boxLabel->setDefaultTextColor(Qt::darkGray);
    boxLabel->setPos(0, m_SpawnBox->sceneBoundingRect().height());
    boxLabel->setOpacity(0.8);

    //Setup the tape scene:
    m_TapeScene = new QGraphicsScene(this);
    m_TapeScene->setSceneRect(0, 0, 50 * 40 + 20, 80);

    //Set up tape view:
    ui->tapeGraphicsView->setFixedSize(1520, 100);
    ui->tapeGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->tapeGraphicsView->setScene(m_TapeScene);
    ui->tapeGraphicsView->horizontalScrollBar()->setValue(ui->tapeGraphicsView->horizontalScrollBar()->minimum());

    //Set up the tape:
    qreal offset = 0;
    for(int i = 0; i < m_TapeLength; i++)
    {
        SquareTapeCell *temp = new SquareTapeCell(QRectF(5 + offset,30, 40, 40));
        m_Tape.append(temp);
        temp->setLabel("-");
        m_TapeScene->addItem(temp);
        offset += 40;
    }

    //Set up tape head:
    m_TapeHead = new TapeHead(this);
    m_TapeScene->addItem(m_TapeHead);
    m_TapeHeadStartXPos = 25 - m_TapeHead->sceneBoundingRect().width()/2.0;
    m_TapeHead->setPos(m_TapeHeadStartXPos, 12);

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
    delete ui;
}

void TuringMachineWindow::clearTapeView()
{
    //Move the tape head back to the start:
    m_TapeHead->setPos(m_TapeHeadStartXPos, 12);

    //Replace all letter in the tape with blanks:
    for(int i = 0; i < m_TapeLength; i++)
        m_Tape[i]->setLabel("-");

    ui->tapeGraphicsView->horizontalScrollBar()->setValue(ui->tapeGraphicsView->horizontalScrollBar()->minimum());
    m_TapeScene->update();
    ui->tapeGraphicsView->update();
}

void TuringMachineWindow::setupSummaryPage()
{
    m_SummaryPageScene = new QGraphicsScene(this);
    m_SummaryPageScene->setSceneRect(QRectF(0, 0, 1501, 601));
    ui->summaryGraphicsView->setScene(m_SummaryPageScene);

    //Summary table view:
    m_SummaryTable = new QTableView;
    m_SummaryTable->setFixedSize(600, 515);
    m_SummaryTable->setFrameStyle(QFrame::Raised);
    m_SummaryTable->setFont(QFont("Corbel", 11));

    m_TableModel = new QStandardItemModel(this);

    //Create the table labels:
    QStringList headerLabels;
    headerLabels.append("From");
    headerLabels.append("To");
    headerLabels.append("Read");
    headerLabels.append("Write");
    headerLabels.append("Move");
    m_TableModel->setHorizontalHeaderLabels(headerLabels);
    m_SummaryTable->setModel(m_TableModel);

    //Summary table label:
    QGraphicsTextItem *tableLabel = m_SummaryPageScene->addText("Summary Table", QFont("Corbel Light"));
    QFont tableLabelFont = tableLabel->font();
    tableLabelFont.setUnderline(true);
    tableLabel->setFont(tableLabelFont);
    tableLabel->setPos(17,5);

    QGraphicsProxyWidget *tableProxy = m_SummaryPageScene->addWidget(m_SummaryTable);
    tableProxy->setPos(0,40);

    //Separator line:
    m_SummaryPageScene->addLine(QLineF(600, 25, 600, 580), QPen(Qt::lightGray));

    //Execution summary table:
    m_ESView = new QGraphicsView;
    m_ESView->setFixedSize(820, 480);
    m_ESView->setFrameStyle(QFrame::Raised);
    m_ESView->setRenderHint(QPainter::Antialiasing);
    QGraphicsProxyWidget *esViewProxy = m_SummaryPageScene->addWidget(m_ESView);
    esViewProxy->setPos(645, 30);

    m_ESScene = new QGraphicsScene(0, 0, 800, 480, this);
    m_ESView->setScene(m_ESScene);

    //Execution summary label:
    QGraphicsTextItem *esTableLabel = m_SummaryPageScene->addText("Execution Summary", QFont("Corbel Light"));
    QFont esTableLabelFont = esTableLabel->font();
    esTableLabelFont.setUnderline(true);
    esTableLabel->setFont(esTableLabelFont);
    esTableLabel->setPos(660,5);

    //Accepted and rejected rects:
    m_AcceptedRect = m_SummaryPageScene->addRect(665, 515, 200, 30, QPen(Qt::lightGray));
    m_CrashedRect = m_SummaryPageScene->addRect(1242, 515, 200, 30, QPen(Qt::lightGray));
    m_AcceptedRect->setBrush(QBrush(Qt::lightGray));
    m_CrashedRect->setBrush(QBrush(Qt::lightGray));

    QGraphicsTextItem *m_AcceptedText =  m_SummaryPageScene->addText("Accepted", QFont("Corbel"));
    m_AcceptedText->setPos(665 + m_AcceptedRect->sceneBoundingRect().width()/2 - m_AcceptedText->sceneBoundingRect().width()/2, 515);
    QGraphicsTextItem *m_RejectedText =  m_SummaryPageScene->addText("Crashed", QFont("Corbel"));
    m_RejectedText->setPos(1242 + m_CrashedRect->sceneBoundingRect().width()/2 - m_RejectedText->sceneBoundingRect().width()/2, 515);

    //Crash reason:
    QGraphicsTextItem *cr = m_SummaryPageScene->addText("Reason for crash:", QFont("Corbel Light"));
    cr->setPos(650, 565);
    m_CrashMessegeEdit = new QLineEdit;
    m_CrashMessegeEdit->setFixedSize(600, 25);
    m_CrashMessegeEdit->setFont(QFont("Corbel Light", 12.0));
    m_CrashMessegeEdit->setText("Reason for crash will appear here");
    m_CrashMessegeEdit->setStyleSheet("background-color: white; color: black;");
    m_CrashMessegeEdit->setEnabled(false);
    QGraphicsProxyWidget *cmeProxy = m_SummaryPageScene->addWidget(m_CrashMessegeEdit);
    cmeProxy->setPos(850, 565);
}

void TuringMachineWindow::setupOptionsPage()
{
    //Page configuration:
    m_OptionsScene = new QGraphicsScene(this);
    m_OptionsScene->setSceneRect(QRectF(0, 0, 1501, 601));
    ui->optionsGraphicsView->setScene(m_OptionsScene);
    ui->optionsGraphicsView->setFont(QFont("Corbel Light"));

    //Save location:
    //m_OptionsScene->addRect(0, 15, 1300, 50, QPen(Qt::gray, 1.0));
    QGraphicsTextItem *slText = m_OptionsScene->addText("Save Location: ", QFont("Corbel Light", 12));
    slText->setDefaultTextColor(Qt::darkGray);
    slText->setPos(10, 25);

    m_SaveLocationLineEdit = new QLineEdit;
    m_SaveLocationLineEdit->setFont(QFont("Corbel Light", 12));
    m_SaveLocationLineEdit->setEnabled(false);
    m_SaveLocationLineEdit->setStyleSheet("background-color: white;");
    m_SaveLocationLineEdit->setText(m_SavePath);
    m_SaveLocationLineEdit->setFixedSize(1000, 40);
    QGraphicsProxyWidget *slleProxy = m_OptionsScene->addWidget(m_SaveLocationLineEdit);
    slleProxy->setPos(180, 20);

    PixmapButton *browseButton = new PixmapButton(this, ":/new/prefix1/Images and Icons/browse icon.png", ":/new/prefix1/Images and Icons/browse icon.png");
    browseButton->setPos(1180, 23);
    browseButton->setScale(0.8);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(getSaveFileLocation()));
    m_OptionsScene->addItem(browseButton);

    //Separator line:
    m_OptionsScene->addLine(110, 85, 1380, 85, QPen(Qt::gray, 1.0));

    //Other options:
    //m_OptionsScene->addRect(0, 100, 1300, 50, QPen(Qt::gray));
    QGraphicsTextItem *psText = m_OptionsScene->addText("Play Speed: ", QFont("Corbel Light", 12));
    psText->setDefaultTextColor(Qt::darkGray);
    psText->setPos(10, 110);

    m_SpeedSpinBox = new QSpinBox;
    m_SpeedSpinBox->setValue(1);
    m_SpeedSpinBox->setFixedSize(80, 40);
    m_SpeedSpinBox->setMinimum(1);
    m_SpeedSpinBox->setMaximum(9);
    connect(m_SpeedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(speedSpinBoxValueChanged(int)));
    QGraphicsProxyWidget *ssbProxy = m_OptionsScene->addWidget(m_SpeedSpinBox);
    ssbProxy->setFont(QFont("Corbel Light", 12));
    ssbProxy->setPos(300, 105);

    //m_OptionsScene->addRect(0, 160, 1300, 50, QPen(Qt::gray));
    QGraphicsTextItem *tlText = m_OptionsScene->addText("Tape Length: ", QFont("Corbel Light", 12));
    tlText->setDefaultTextColor(Qt::darkGray);
    tlText->setPos(10, 170);

    m_TapeLengthSpinBox = new QSpinBox;
    m_TapeLengthSpinBox->setValue(20);
    m_TapeLengthSpinBox->setFixedSize(80, 40);
    m_TapeLengthSpinBox->setMinimum(50);
    m_TapeLengthSpinBox->setMaximum(200);
    connect(m_TapeLengthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(tapeLengthSpinBoxValueChanged(int)));
    QGraphicsProxyWidget *tlsbProxy = m_OptionsScene->addWidget(m_TapeLengthSpinBox);
    tlsbProxy->setFont(QFont("Corbel Light", 12));
    tlsbProxy->setPos(300, 165);

    //m_OptionsScene->addRect(0, 220, 1300, 50, QPen(Qt::gray));
    QGraphicsTextItem *ahcText = m_OptionsScene->addText("Arrow Highlight Color: ", QFont("Corbel Light", 12));
    ahcText->setDefaultTextColor(Qt::darkGray);
    ahcText->setPos(10, 230);

    //AHC label and color box:
    m_OptionsScene->addRect(300, 225, 200, 40, QPen(Qt::gray));
    m_AHCButton = new ColorButton(QRectF(525, 225, 50, 40), this);
    m_AHCButton->setBrush(QBrush(m_AHCColor));
    m_AHCButton->setPen(QPen(m_AHCColor));
    connect(m_AHCButton, SIGNAL(clicked()), this, SLOT(changeAHCColor()));
    m_OptionsScene->addItem(m_AHCButton);

    m_AHCColorLabel = m_OptionsScene->addText(m_AHCColor.name(), QFont("corbel Light", 14));
    m_AHCColorLabel->setDefaultTextColor(Qt::darkGray);
    m_AHCColorLabel->setPos(305, 226);

    //m_OptionsScene->addRect(0, 280, 1300, 50, QPen(Qt::gray));
    QGraphicsTextItem *spText = m_OptionsScene->addText("Current State Color: ", QFont("Corbel Light", 12));
    spText->setDefaultTextColor(Qt::darkGray);
    spText->setPos(10, 290);

    m_OptionsScene->addRect(300, 285, 200, 40, QPen(Qt::gray));
    m_CSCButton = new ColorButton(QRectF(525, 285, 50, 40), this);
    m_CSCButton->setBrush(QBrush(m_CSCColor));
    m_CSCButton->setPen(QPen(m_CSCColor));
    connect(m_CSCButton, SIGNAL(clicked()), this, SLOT(changeCSCColor()));
    m_OptionsScene->addItem(m_CSCButton);

    m_CSCColorLabel = m_OptionsScene->addText(m_CSCColor.name(), QFont("corbel Light", 14));
    m_CSCColorLabel->setDefaultTextColor(Qt::darkGray);
    m_CSCColorLabel->setPos(305, 286);
}

void TuringMachineWindow::setupHelpPage()
{
    QString licenseMessage = "";
    QString licenseMessage2 = "";
    //Get license data from the files:
        //GNU License:
    QFile file1("COPYING.txt");
    if(file1.open(QIODevice::ReadOnly))
    {
        QTextStream instream(&file1);
        licenseMessage.append(instream.readAll());
        file1.close();
    }
        //LGPL License:
    QFile file2("COPYING.LESSER");
    if(file2.open(QIODevice::ReadOnly))
    {
        QTextStream instream(&file2);
        licenseMessage2.append(instream.readAll());
        file2.close();
    }

    //Page configurations:
    m_HelpScene = new QGraphicsScene(this);
    m_HelpScene->setSceneRect(QRectF(0, 0, 1501, 601));
    ui->helpGraphicsView->setScene(m_HelpScene);
    ui->helpGraphicsView->setFont(QFont("Corbel Light"));

    //About section:
    QGraphicsTextItem *aboutMainLabel = m_HelpScene->addText("About");
    QFont aboutLabelFont;
    aboutLabelFont.setFamily("Corbel Light");
    aboutLabelFont.setPointSizeF(15.0);
    aboutLabelFont.setItalic(true);
    aboutMainLabel->setFont(aboutLabelFont);
    aboutMainLabel->setDefaultTextColor(Qt::darkGray);
    aboutMainLabel->setPos(720, 5);

        //Separator line:
    m_HelpScene->addLine(10, 35, 1480, 35, QPen(Qt::gray, 1.0));

    //About box:234
    //m_HelpScene->addRect(360, 35, 760, 40, QPen(Qt::gray, 0.5));
        //About app button:
    SquareButton *aboutAppButton = new SquareButton(this, QRectF(364, 40, 250, 30), "About App");
    connect(aboutAppButton, SIGNAL(clicked()), this, SLOT(aboutAppButtonClicked()));
    m_HelpScene->addItem(aboutAppButton);
        //About Qt button:
    SquareButton *aboutQTButton = new SquareButton(this, QRectF(615, 40, 250, 30), "About Qt");
    connect(aboutQTButton, SIGNAL(clicked()), this, SLOT(aboutQtButtonClicked()));
    m_HelpScene->addItem(aboutQTButton);
        //About Author button:
    SquareButton *aboutAuthorButton = new SquareButton(this, QRectF(866, 40, 250, 30), "About Author");
    connect(aboutAuthorButton, SIGNAL(clicked()), this, SLOT(aboutAuthorButtonClicked()));
    m_HelpScene->addItem(aboutAuthorButton);

    //Separator line:
    m_HelpScene->addLine(10, 75, 1480, 75, QPen(Qt::gray, 1.0));

    //License section:
    QGraphicsTextItem *licenseMainLabel = m_HelpScene->addText("License");
    QFont licenseMainFont;
    licenseMainFont.setFamily("Corbel Light");
    licenseMainFont.setPointSizeF(15.0);
    licenseMainFont.setItalic(true);
    licenseMainLabel->setFont(licenseMainFont);
    licenseMainLabel->setDefaultTextColor(Qt::darkGray);
    licenseMainLabel->setPos(710, 85);
        //Lisence Text Edit:
    m_HelpScene->addRect(10, 120, 1470, 300, QPen(Qt::gray));
    QTextEdit *licenseTextEdit1 = new QTextEdit;
    licenseTextEdit1->setFixedSize(700, 288);
    licenseTextEdit1->setStyleSheet("background-color: white");
    licenseTextEdit1->setFrameStyle(QFrame::Raised);
    licenseTextEdit1->setText(licenseMessage);
    licenseTextEdit1->setFont(QFont("Corbel", 10));
    licenseTextEdit1->setReadOnly(true);
    QGraphicsProxyWidget *lteProxy = m_HelpScene->addWidget(licenseTextEdit1);
    lteProxy->setPos(40, 122);

    QTextEdit *licenseTextEdit2 = new QTextEdit;
    licenseTextEdit2->setFixedSize(700, 288);
    licenseTextEdit2->setStyleSheet("background-color: white");
    licenseTextEdit2->setFrameStyle(QFrame::Raised);
    licenseTextEdit2->setText(licenseMessage2);
    licenseTextEdit2->setFont(QFont("Corbel", 10));
    licenseTextEdit2->setReadOnly(true);
    QGraphicsProxyWidget *lte2Proxy = m_HelpScene->addWidget(licenseTextEdit2);
    lte2Proxy->setPos(750, 122);

    //Separator line:
    m_HelpScene->addLine(10, 115, 1480, 115, QPen(Qt::gray, 1.0));

    //User manual box:
    QGraphicsTextItem *manualLabel = m_HelpScene->addText("User Manual", licenseMainFont);
    manualLabel->setDefaultTextColor(Qt::darkGray);
    manualLabel->setPos(690, 430);

    //Separator line:
    m_HelpScene->addLine(10, 460, 1480, 460, QPen(Qt::gray, 1.0));
    m_HelpScene->addRect(10, 465, 1470, 136, QPen(Qt::gray));

    //Manual Button:
    SquareButton *openManualButton = new SquareButton(this, QRectF(10, 466, 500, 134), "Open User Manual");
    connect(openManualButton, SIGNAL(clicked()), this, SLOT(openUserManualButtonClicked()));
    m_HelpScene->addItem(openManualButton);
        //Logo
    QPixmap logoPic = QPixmap(":/new/prefix1/Images and Icons/sim3.png");
    QGraphicsPixmapItem *logo = m_HelpScene->addPixmap(logoPic.scaled(logoPic.width()/2.8, logoPic.height()/2.8));
    logo->setScale(0.8);
    logo->setOpacity(0.2);
    logo->setPos(680, 460);
    QGraphicsTextItem *appName = m_HelpScene->addText("TM SIMULATOR");
    QFont appNameFont;
    appNameFont.setFamily("Corbel Light");
    appNameFont.setStretch(200);
    appNameFont.setPointSizeF(20.0);
    appName->setFont(appNameFont);
    appName->setOpacity(0.2);
    appName->setPos(850, 520);
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
        m_SummaryTable->setRowHeight(i, 35);
    }
}

void TuringMachineWindow::displayTestSummary()
{
    //Display the crash messege if any:
    QString crashMessege = m_Processor->getCrashString();
    if(crashMessege == "")
        m_CrashMessegeEdit->setText("Input was accepted. No crash messge.");
    else
        m_CrashMessegeEdit->setText(crashMessege);

    //Retrieve and display the tape and transition records:
    QStringList transitionRecord = m_Processor->getTransitionRecord().split('\n', Qt::SkipEmptyParts);
    QStringList tapeRecord = m_Processor->getTapeRecord();

    if(!transitionRecord.isEmpty() && !tapeRecord.isEmpty())
    {
        m_ESScene->clear();
        //Declare variables:
        QStringList recordDetail;
        QStringList transitionEntry;
        QGraphicsTextItem *tempText;
        QTextEdit *textEdit;
        QFont inpTextFont;
        int height = 400;
        int yOffset = 0;
        int index = 0;

        //Set up the font:
        inpTextFont.setFamily("Corbel");
        inpTextFont.setLetterSpacing(QFont::AbsoluteSpacing, 5);

        //Set up the scene size:
        if(tapeRecord.length() > 10)
            height = 45 * tapeRecord.length();
        m_ESScene->setSceneRect(0, 0, 800, height);

        //Display all test result data:
        for(int i = 0; i< tapeRecord.length(); i++)
        {
            //Tape record:
            m_ESScene->addRect(10, 5 + yOffset, 470, 35, QPen(Qt::lightGray));
            recordDetail = tapeRecord[i].split('-', Qt::SkipEmptyParts);
            index = recordDetail[1].toInt();

            //Create the line edit item and set show it on the scene:
            textEdit = new QTextEdit;
            textEdit->setFixedSize(400, 30);
            textEdit->setFrameStyle(QFrame::Raised);
            textEdit->setFont(inpTextFont);
            textEdit->setText(recordDetail[0]);

            //Change the color of the letter at the index to red:
            QTextCursor cursor = textEdit->textCursor();
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index);
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
            QTextCharFormat chFormat;
            chFormat.setTextOutline(QPen(Qt::red));
            chFormat.setFontWeight(1);
            cursor.setCharFormat(chFormat);

            QGraphicsProxyWidget *textEditProxy = m_ESScene->addWidget(textEdit);
            textEditProxy->setPos(12, 7 + yOffset);

            if(i < tapeRecord.length() - 1)
            {
                //Transition data:
                transitionEntry = transitionRecord[i].split(',', Qt::SkipEmptyParts);

                //From state:
                m_ESScene->addRect(495, 5 + yOffset, 50, 35, QPen(Qt::lightGray));
                tempText = m_ESScene->addText(transitionEntry[0]);
                tempText->setPos(498, 7 + yOffset);
                //To state:
                m_ESScene->addRect(555, 5 + yOffset, 50, 35, QPen(Qt::lightGray));
                tempText = m_ESScene->addText(transitionEntry[1]);
                tempText->setPos(557, 7 + yOffset);
                //Read:
                m_ESScene->addRect(615, 5 + yOffset, 50, 35, QPen(Qt::lightGray));
                tempText = m_ESScene->addText(transitionEntry[2]);
                tempText->setPos(617, 7 + yOffset);
                //Write:
                m_ESScene->addRect(675, 5 + yOffset, 50, 35, QPen(Qt::lightGray));
                tempText = m_ESScene->addText(transitionEntry[3]);
                tempText->setPos(677, 7 + yOffset);
                //Move
                m_ESScene->addRect(735, 5 + yOffset, 50, 35, QPen(Qt::lightGray));
                tempText = m_ESScene->addText(transitionEntry[4]);
                tempText->setPos(737, 7 + yOffset);
                yOffset += 45;
            }
        }

        //Set accept or reject color on box:
        if(crashMessege == "")
        {
            m_AcceptedRect->setBrush(QBrush(Qt::green));
            m_CrashedRect->setBrush(QBrush(Qt::lightGray));
        }
        else
        {
            m_CrashedRect->setBrush(QBrush(Qt::red));
            m_AcceptedRect->setBrush(QBrush(Qt::lightGray));
        }
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
        QString sp = inStream.readLine();
        m_Speed = sp.toInt();
        m_AHCColor = QColor(inStream.readLine());
        m_CSCColor = QColor(inStream.readLine());
        loadFile.close();
    }
}

void TuringMachineWindow::quitApp()
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
            outStream << m_Speed << '\n';
            outStream << m_AHCColorLabel->toPlainText() << '\n';
            outStream << m_CSCColorLabel->toPlainText() << '\n';
            saveFile.close();
        }
    }

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

            m_TapeHead->setPos(25 - m_TapeHead->sceneBoundingRect().width()/2.0, 12);
            this->play();

            //Display the test summary:
            this->displayTestSummary();
        }
        else
        {
            QString message = "There is a possible infinite loop in your TM.\n"
                              "100000 read, write and move iterations where executed.\n"
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
    m_ESScene->clear();
    m_FileLoaded = false;
    m_LoadedFile = "";
    m_LoadedDescription = "";

    //Reset the summary page details:
    m_AcceptedRect->setBrush(QBrush(Qt::lightGray));
    m_CrashedRect->setBrush(QBrush(Qt::lightGray));
    m_NumOfStates = 0;
    m_CrashMessegeEdit->setText("Reason for crash will appear here");
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
    ui->clearPushButton->setEnabled(false);
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
        ui->clearPushButton->setEnabled(true);
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
    if(m_MoveCounter < 40)
    {
        m_TapeHead->setPos(m_TapeHead->scenePos().x() - 1, m_TapeHead->scenePos().y());
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
    if(m_MoveCounter < 40)
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
    QDir loc(sfl);
    if(loc.exists())
        m_SavePath = sfl;
    m_SaveLocationLineEdit->setText(sfl);
    m_SettingsChanged = true;
}

void TuringMachineWindow::changeAHCColor()
{
    QColor newColor = QColorDialog::getColor(m_AHCColor, this, "Pick a color");
    if(newColor.isValid())
    {
        m_AHCButton->setBrush(QBrush(newColor));
        m_AHCButton->setPen(QPen(newColor));
        m_AHCColor = newColor;
        m_AHCColorLabel->setPlainText(m_AHCColor.name());

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
        m_CSCButton->setBrush(QBrush(newColor));
        m_CSCButton->setPen(QPen(newColor));
        m_CSCColor = newColor;
        m_CSCColorLabel->setPlainText(m_CSCColor.name());
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

    //Clear the scene:
    for(SquareTapeCell *s : m_Tape)
        m_TapeScene->removeItem(s);
    m_Tape.clear();

    //Resize the scene:
    int sceneWidth = m_TapeLength * 40 + 20;
    m_TapeScene->setSceneRect(0, 0, sceneWidth, 80);

    //Set up the tape:
    qreal offset = 0;
    for(int i = 0; i < m_TapeLength; i++)
    {
        SquareTapeCell *temp = new SquareTapeCell(QRectF(5 + offset,30, 40, 40));
        m_Tape.append(temp);
        temp->setLabel("-");
        m_TapeScene->addItem(temp);
        offset += 40;
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
                      "Occupation:\tStudent at UNISA\n"
                      "Email:\tMalone.mkd.makoto@gmail.com\n\n"
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

