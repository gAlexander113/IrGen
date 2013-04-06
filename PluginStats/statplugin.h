#ifndef STATPLUGIN_H
#define STATPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../IrGene_Qt_5_0_1/statinterface.h"

class StatPlugin : public QObject, StatInterface
{
    Q_OBJECT
    Q_INTERFACES(StatInterface)
    Q_PLUGIN_METADATA(IID "simple.com.IrGene_Qt_5_0_1.StatInterface")
public:
    StatPlugin();
    virtual QString name() const;
    virtual QByteArray statistics(DataBox *data); // считаем Xi Phi C P Ka
};

#endif // STATPLUGIN_H
