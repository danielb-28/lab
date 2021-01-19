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
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += static

INCLUDEPATH += "./boost_1_75_0"

DEPENDPATH += "./boost_1_75_0"

#LIBS += -L"C:\Cpp_Libs\boost_1_73_0\stage\lib" \
#        -llibboost_iostreams-vc142-mt-x64-1_73 \
#        -llibboost_chrono-vc142-mt-x64-1_73 \
#        -llibboost_system-vc142-mt-x64-1_73 \
#        -llibboost_wserialization-vc142-mt-x64-1_73 \
#        -llibboost_timer-vc142-mt-x64-1_73 \
#        -llibboost_filesystem-vc142-mt-x64-1_73 \
#        -llibboost_date_time-vc142-mt-x64-1_73 \
#        -llibboost_thread-vc142-mt-x64-1_73 \
#        -llibboost_regex-vc142-mt-x64-1_73

LIBS += -L"./boost_1_75_0/libs" \

win32 {


}

DISTFILES +=
