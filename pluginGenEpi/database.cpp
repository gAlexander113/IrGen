#include "database.h"


QString DataBase::name() const
{
    return tr("GenEpi");
}

bool DataBase::connectDataBase()
{    
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("afreq");
    db.setHostName("89.249.160.35");
    db.setUserName(user);
    db.setPassword(pass);
    if (!db.open())
    {
        qDebug() << "cannot open database:" << db.lastError();
        return false;
    }
    return true;
}

void DataBase::getQuery()
{
    if (!connectDataBase())
    {
        return;
    }
    else
        qDebug() << "connection success";

    QSqlQuery query(db);


    QStringList list = db.tables();
    foreach (QString str, list) {
        qDebug() << "Table: " << str;
    }

    QString sqlCom =
            "SELECT Gene, Allele"
            "FROM af";
//            "WHERE Illness = 1";
    if (!query.exec(sqlCom))
    {
        qDebug() << "cannot SELECT" << query.lastError();
        return;
    }

    QSqlRecord rec = query.record();
    QString gene;
    QString allele;

    while (query.next())
    {
        gene = query.value(rec.indexOf("Gene")).toString();
        allele = query.value(rec.indexOf("Allele")).toString();
        qDebug() << gene << allele;
    }
}

void DataBase::setUser(QString username, QString password)
{
    user = username;
    pass = password;
}


DataBase::~DataBase()
{
}


DataBase::DataBase()
{
}
