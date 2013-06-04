TEMPLATE = lib
CONFIG  += plugin
QT      += \
    widgets \
    xml \
    sql
HEADERS += \
    conttblpugin.h \
    ../IrGene_Qt_5_0_1/conttblinterface.h

SOURCES += \
    conttblpugin.cpp
TARGET  = $$qtLibraryTarget(pluginOneAllele)

DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins
