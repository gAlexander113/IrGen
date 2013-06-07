TEMPLATE = lib
CONFIG  += plugin
QT      += \
    widgets \
    xml \
    sql
HEADERS += \
    database.h \
    ../IrGene_Qt_5_0_1/databaseinterface.h


SOURCES += \
    database.cpp

TARGET  = $$qtLibraryTarget(pluginGenEpi)

DESTDIR = ..\IrGene_Qt_5_0_1-build-Desktop_Qt_5_0_1_MinGW_32bit\debug\plugins
