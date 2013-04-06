#ifndef CONTTBLPLUGIN_H
#define CONTTBLPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../IrGene_Qt_5_0_1/conttblinterface.h"

class ContTblPlugin : public QObject, ContInterface
{
    Q_OBJECT
    Q_INTERFACES(ContInterface)
    Q_PLUGIN_METADATA(IID "simple.com.IrGene_Qt_5_0_1.ContInterface")
private:
    QString confInt;
    double RR;
    void processRR(QString allele, QVector<QString> names, QVector<int> numH, QVector<int> numI);
public:
    ContTblPlugin();
    virtual QString name() const;
    virtual void tblResult(DataBox *data, QString nameGene);

};

#endif // CONTTBLPLUGIN_H
