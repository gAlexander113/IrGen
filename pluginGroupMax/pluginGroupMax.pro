TEMPLATE = lib
CONFIG  += plugin
QT      += \
    widgets \
    xml
HEADERS += \
    conttblplugin.h \
    ../IrGene_Qt_5_0_1/conttblinterface.h \
    conttblplugin.h

SOURCES += \
    conttblplugin.cpp
TARGET  = $$qtLibraryTarget(pluginGroupMax)
DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins
