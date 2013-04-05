#ifndef CONTTBLINTERFACE_H
#define CONTTBLINTERFACE_H

#include "main.h"
#include "databox.h"
#include <QString>
#include <QtCore/qmath.h>

class ContInterface //интерфейс для таблиц сопряженностей
{
public:
    virtual QString name() const = 0;
    virtual void tblResult(DataBox *data, QString nameGene) = 0;
};

Q_DECLARE_INTERFACE(ContInterface, "simple.com.IrGene_Qt_5_0_1.ContInterface")

#endif // CONTTBLINTERFACE_H
