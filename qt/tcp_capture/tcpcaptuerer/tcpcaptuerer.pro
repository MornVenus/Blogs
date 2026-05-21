QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    packdataservice.cpp \
    tcpcaptureservice.cpp \
    tcppacket.cpp \
    views/powerview.cpp

HEADERS += \
    mainwindow.h \
    models.h \
    myfiltermodel.h \
    packdataservice.h \
    packdelegate.h \
    packlistmodel.h \
    tcpcaptureservice.h \
    tcppacket.h \
    views/powerview.h

FORMS += \
    mainwindow.ui \
    views/powerview.ui

INCLUDEPATH += D:/softwares/Libs/NpcapSDK/Include
LIBS += -LD:/softwares/Libs/NpcapSDK/Lib/x64
LIBS += -lwpcap
LIBS += -lole32
LIBS += -lPacket
LIBS += -lIphlpapi
LIBS += -lWs2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
