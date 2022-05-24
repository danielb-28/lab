QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

QMAKE_CXXFLAGS += -Ofast

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    comando_serial.cpp \
    porta_serial.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h \
    comando_serial.h \
    porta_serial.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += static

DEFINES += QCUSTOMPLOT_USE_OPENGL

#INCLUDEPATH += "./boost_1_75_0"

#DEPENDPATH += "./boost_1_75_0"

#LIBS += -L"./boost_1_75_0/libs" \
LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system
DISTFILES +=
