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

#ifndef TURINGMACHINEWINDOW_H
#define TURINGMACHINEWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QTableView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QtGui>
#include "mystateitem.h"
#include "squarebutton.h"
#include "squarespawnbox.h"
#include "turingmachine.h"
#include "tmprocessor.h"
#include "squaretapecell.h"
#include "tapecell.h"
#include "tapehead.h"
#include "colorbutton.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TuringMachineWindow; }
QT_END_NAMESPACE

class TuringMachineWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Constructor and destructor:
    TuringMachineWindow(QWidget *parent = nullptr);
    ~TuringMachineWindow();

    //Mutator member functions:
    void clearTapeView();
    void setupSummaryPage();
    void setupOptionsPage();
    void setupHelpPage();
    void populateSummaryTable(QStringList tableData);
    void displayTestSummary();
    void loadSettings();
    void quitApp();

signals:
    void doneMoving();

private slots:

    void exitButtonClicked();

    void addArrowButtonClicked();

    void addLoopArrowButtonClicked();

    void addStateButtonClicked();

    void deleteStateButtonClicked();

    void deleteArrowButtonClicked();

    void buildButtonClicked();

    void setHALTStateButtonClicked();

    void setSTARTStateButtonClicked();

    void testInputButtonClicked();

    void clearSceneButtonClicked();

    void on_inputLineEdit_editingFinished();

    void moveTapeHead();

    void moveLeft();

    void moveRight();

    void play();

    void unpause();

    void on_clearPushButton_clicked();

    void on_actionSaveTM_triggered();

    void on_actionLoadTM_triggered();

    void on_actionExit_triggered();

    void getSaveFileLocation();

    void changeAHCColor();

    void changeCSCColor();

    void speedSpinBoxValueChanged(int val);

    void tapeLengthSpinBoxValueChanged(int val);

    void aboutAppButtonClicked();

    void aboutQtButtonClicked();

    void aboutAuthorButtonClicked();

    void getSaveDetails(QStringList details);

    void fadeOutDescription();

    void openUserManualButtonClicked();

    void fadeOutLogo();

private:
    Ui::TuringMachineWindow *ui;
    QGraphicsScene *m_Scene;
    QGraphicsScene *m_TapeScene;
    QGraphicsScene *m_SummaryPageScene;
    QGraphicsScene *m_ESScene;
    QGraphicsScene *m_OptionsScene;
    QGraphicsScene *m_HelpScene;
    QGraphicsScene *m_SlidesScene;
    SquareSpawnBox *m_SpawnBox;
    TapeHead *m_TapeHead;
    QTimer *m_MoveLeftTimer;
    QTimer *m_MoveRightTimer;
    QTimer *m_PauseTimer;
    QTimer *m_DescTimer;
    QTimer *m_LogoTimer;
    QPointF m_PrevHeadPos;
    qreal m_TapeHeadStartXPos;
    QList<MyStateItem*> m_TM;
    QList<SquareTapeCell*> m_Tape;
    QList<int> m_MachineData;
    QStringList m_TapeData;
    QStringList m_SummaryTableData;

    QLineEdit *m_CrashMessegeEdit;
    QLineEdit *m_ETMessegeEdit;
    QLineEdit *m_SaveLocationLineEdit;
    QString m_SavePath;
    QString m_LoadedFile;
    QString m_LoadedDescription;
    QTextEdit *m_TMDescriptionTextEdit;
    QGraphicsProxyWidget *m_DescEditProxy;
    QGraphicsTextItem *m_DescriptionLabel;
    QGraphicsPixmapItem *m_Logo;

    TuringMachine *m_TMModel;
    TMProcessor *m_Processor;

    QSpinBox *m_SpeedSpinBox;
    QSpinBox *m_TapeLengthSpinBox;
    ColorButton *m_AHCButton;
    ColorButton *m_CSCButton;
    QColor m_AHCColor;
    QColor m_CSCColor;
    QGraphicsTextItem *m_AHCColorLabel;
    QGraphicsTextItem *m_CSCColorLabel;

    QTableView *m_SummaryTable;
    QStandardItemModel *m_TableModel;
    QGraphicsView *m_ESView;

    QGraphicsRectItem *m_AcceptedRect;
    QGraphicsRectItem *m_CrashedRect;

    int m_NumOfStates;
    int m_CurrentCell;
    int m_Count;
    int m_TapeCounter;
    int m_MoveCounter;
    int m_TapeLength;
    bool m_HasHALTState;
    bool m_HasSTARTState;
    bool m_SettingsChanged;
    bool m_Modified;
    bool m_FileLoaded;
    qreal m_DisplayCounter;
    qreal m_Speed;
    qreal m_TimeCounter;
};

#endif // TURINGMACHINEWINDOW_H
