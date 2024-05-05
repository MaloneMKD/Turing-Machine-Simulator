QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colorbutton.cpp \
    looparrow.cpp \
    main.cpp \
    mystateitem.cpp \
    pixmapbutton.cpp \
    popupmessagebox.cpp \
    qgraphicsroundedrectitem.cpp \
    savedialog.cpp \
    solidarrow.cpp \
    squarebutton.cpp \
    squarespawnbox.cpp \
    squaretapecell.cpp \
    tapecell.cpp \
    tapehead.cpp \
    tmedge.cpp \
    tmprocessor.cpp \
    tmsscene.cpp \
    tmstate.cpp \
    turingmachine.cpp \
    turingmachinewindow.cpp

HEADERS += \
    colorbutton.h \
    looparrow.h \
    mystateitem.h \
    pixmapbutton.h \
    popupmessagebox.h \
    qgraphicsroundedrectitem.h \
    savedialog.h \
    solidarrow.h \
    squarebutton.h \
    squarespawnbox.h \
    squaretapecell.h \
    tapecell.h \
    tapehead.h \
    tmedge.h \
    tmprocessor.h \
    tmsscene.h \
    tmstate.h \
    tmsview \
    tmsview \
    turingmachine.h \
    turingmachinewindow.h

FORMS += \
    popupmessagebox.ui \
    savedialog.ui \
    turingmachinewindow.ui

# Application icon
RC_FILE = TuringMachineSimulator.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ImagesAndIcons.qrc
