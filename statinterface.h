#ifndef STATINTERFACE_H
#define STATINTERFACE_H

#include <QString>
#include "databox.h"

class StatInterface
{
public:
    virtual QString name() const = 0;
    virtual QByteArray statistics(DataBox *data) = 0;
};

Q_DECLARE_INTERFACE(StatInterface, "simple.com.IrGene_Qt_5_0_1.StatInterface")

#endif // STATINTERFACE_H
