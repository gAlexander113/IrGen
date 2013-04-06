#ifndef CONTTBLPUGIN_H
#define CONTTBLPUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../IrGene_Qt_5_0_1/conttblinterface.h"

class ContTblPugin : public QObject, public ContInterface
{
    Q_OBJECT
    Q_INTERFACES(ContInterface)
    Q_PLUGIN_METADATA(IID "simple.com.IrGene_Qt_5_0_1.ContInterface" )
private:
    QString confInt;
    double RR;
    void processRR(QString allele, Gene gene);
public:
    ContTblPugin(){}
    virtual QString name() const;
    virtual void tblResult(DataBox *data, QString nameGene);
};

#endif // CONTTBLPUGIN_H
