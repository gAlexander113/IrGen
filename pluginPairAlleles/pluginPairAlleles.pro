TEMPLATE = lib
CONFIG  += plugin
QT      += \
    widgets \
    xml
HEADERS += \
    conttblplugin.h \
    ../IrGene_Qt_5_0_1/conttblinterface.h

SOURCES += \
    conttblplugin.cpp
TARGET  = $$qtLibraryTarget(pluginPairAlleles)
DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins
