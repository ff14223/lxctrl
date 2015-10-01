TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lconfig++

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
    src/main.cpp

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
    inc/IDigitalSignal.h

OTHER_FILES += \
    src/vds/vdsframe.wth \
    src/vds/vds.c.txt \
    lxctrl.cfg

