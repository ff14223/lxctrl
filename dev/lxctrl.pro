TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CCFLAG += -std=c++11
INCLUDEPATH += /usr/include/mysql
QMAKE_CXXFLAGS += -pthread
LIBS += -lconfig++ -lmysqlclient  -pthread

SOURCES += src/settings.cpp \
    src/vds/vds.cpp \
    src/vds/vds2465.c \
    src/vds/vdsAlarm.c \
    src/vds/vdsdecode.c \
    src/vds/vdsFrame.c \
    src/ioimage.cpp \
    src/vds/digitalsignal.cpp \
    src/vdsframe.cpp \
    src/test.cpp \
    src/main.cpp \
    src/mysqladapter.cpp \
    src/systeminit.cpp \
    src/controler.cpp \
    src/alarm.cpp \
    src/bmzuser.cpp \
    src/bmzuserstatus.cpp \
    src/dbobject.cpp \
    src/terminal.cpp \
    src/canio.cpp \
    src/cannode.cpp

HEADERS += \
    src/settings.h \
    vds.h \
    src/vds/vds.h \
    src/vds/vds2465.h \
    src/vds/vdsFrame.h \
    src/ioimage.h \
    src/vds/digitalsignal.h \
    src/vdsframe.h \
    src/test.h \
    src/lxctrl.h \
    inc/IDigitalInputs.h \
    inc/IDigitalOutputs.h \
    inc/IDigitalSignal.h \
    src/mysqladapter.h \
    inc/IDatabase.h \
    inc/interfaces.h \
    inc/ISystemData.h \
    inc/IIoImage.h \
    src/alarm.h \
    inc/IAlarm.h \
    inc/IBmzUser.h \
    src/bmzuser.h \
    inc/ILogger.h \
    inc/ibmzuserstatus.h \
    inc/bmzuserstatus.h \
    src/bmzuserstatus.h \
    src/dbobject.h \
    src/canio.h \
    src/cannode.h \
    inc/iiobus.h \
    inc/termcolor.h

OTHER_FILES += \
    src/vds/vdsframe.wth \
    src/vds/vds.c.txt \
    lxctrl.cfg

DISTFILES += \
    how-to.txt

