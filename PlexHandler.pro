#-------------------------------------------------
#
# Project created by QtCreator 2019-08-26T11:15:12
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += opengl
LIBS     += -lopengl32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlexHandler
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    datareader.cpp \
    dataprocessor.cpp \
    port.cpp \
    windfile.cpp \
    windSettings.cpp \
    radar.cpp \
    glgraph.cpp

HEADERS += \
        mainwindow.h \
    spsprotocol.h \
    datareader.h \
    dataprocessor.h \
    port.h \
    windfile.h \
    windSettings.h \
    radar.h \
    glgraph.h

FORMS += \
        mainwindow.ui \
    windport.ui \
    windfile.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./ -lMghdlc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -lMghdlcd
else:unix:!macx: LIBS += -L$$PWD/./ -lMghdlc

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./libMghdlc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./libMghdlcd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./Mghdlc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./Mghdlcd.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/./libMghdlc.a
