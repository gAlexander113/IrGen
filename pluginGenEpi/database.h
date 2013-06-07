#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtPlugin>
#include "../IrGene_Qt_5_0_1/databaseinterface.h"

class DataBase : public QObject, DataBaseInterface
{
    Q_OBJECT
    Q_INTERFACES(DataBaseInterface)
    Q_PLUGIN_METADATA(IID "simple.com.IrGene_Qt_5_0_1.DataBaseInterface")
public:
    DataBase();
    ~DataBase();
    QString name() const;    
    void getQuery();
    void setUser(QString username, QString password);
private:
    QSqlDatabase db;
    QString user;
    QString pass;

    bool connectDataBase();
};

#endif // DATABASE_H
