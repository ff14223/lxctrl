TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/mysql

LIBS += -lconfig++ -lmysqlclient

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
    src/systeminit.cpp

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
    inc/IIoImage.h

OTHER_FILES += \
    src/vds/vdsframe.wth \
    src/vds/vds.c.txt \
    lxctrl.cfg

