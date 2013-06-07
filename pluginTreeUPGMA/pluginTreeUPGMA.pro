TEMPLATE = lib
CONFIG  += plugin
QT      += \
    widgets \
    xml \
    sql \
    svg
HEADERS += \
    ../IrGene_Qt_5_0_1/conttblinterface.h \
    treeplugin.h \
    graphicstextitem.h


TARGET  = $$qtLibraryTarget(pluginTreeUPGMA)

DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins

SOURCES += \
    treeplugin.cpp \
    graphicstextitem.cpp
