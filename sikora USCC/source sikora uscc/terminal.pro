QT += widgets serialport

TARGET = terminal
TEMPLATE = app

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)
include(./lib_snap7.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    linesetting.cpp \
    mylineedit.cpp \
    snap7.cpp \
    plc_siemens.cpp \
    logsetting.cpp \
    chngepassword.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    sleep.h \
    linesetting.h \
    files.h \
    mylineedit.h \
    flashingpushbutton.h \
    tabletext.h \
    snap7.h \
    plc_siemens.h \
    logsetting.h \
    chngepassword.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    linesetting.ui \
    logsetting.ui \
    chngepassword.ui

RESOURCES += \
    terminal.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
