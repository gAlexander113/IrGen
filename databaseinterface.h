#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include <QString>
#include "databox.h"

class DataBaseInterface
{
public:
    virtual QString name() const = 0;
    virtual void setUser(QString username, QString password) = 0;
    virtual void getQuery() = 0;
};

Q_DECLARE_INTERFACE(DataBaseInterface, "simple.com.IrGene_Qt_5_0_1.DataBaseInterface")

#endif // DATABASEINTERFACE_H
