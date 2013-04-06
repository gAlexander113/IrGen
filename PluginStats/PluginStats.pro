TEMPLATE = lib
CONFIG  += plugin
QT      += \
    xml \
    widgets
HEADERS += \
    statplugin.h \
    ../IrGene_Qt_5_0_1/statinterface.h

SOURCES += \
    statplugin.cpp
TARGET  = $$qtLibraryTarget(pluginStats)
DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins
