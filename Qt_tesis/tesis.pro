#-------------------------------------------------
#
# Project created by QtCreator 2012-10-26T15:08:10
#
#-------------------------------------------------

QT       += core gui phonon


TARGET = tesis
TEMPLATE = app

QMAKE_CXXFLAGS+=-fpermissive

SOURCES += main.cpp\
    SlidingStackedWidget.cpp \
    serial_helper.c \
    QKeyPushButton.cpp \
    functions.cpp \
    examplemyfocus.cpp \
    abstractmeter.cpp \
    widgetwithbackground.cpp \
    widgetKeyBoard.cpp \
    MainWindow.cpp \
    carevent.cpp


HEADERS  += MainWindow.h \
    SlidingStackedWidget.h \
    serial_helper.h \
    QKeyPushButton.h \
    functions.h \
    examplemyfocus.h \
    abstractmeter.h \
    widgetwithbackground.h \
    widgetKeyBoard.h \
    customtm.h \
    carevent.h

RESOURCES += \
    resources.qrc

OTHER_FILES +=

FORMS +=


