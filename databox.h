#ifndef DATABOX_H
#define DATABOX_H

#include "main.h"
#include <QtXml>
#include <QtSql>

class DataBox : public QAbstractTableModel
{
    Q_OBJECT
private:
    int cl_nrow;
    int cl_ncolumn;
    int cl_activePop; //текущая активная популяция
    QVector<Population> cl_genes;
    QVector<DataForAlleles> cl_data; //преобразованные данные, вывод в таблицу
    QImage *im;
    void traverseNode(const QDomNode& node);
public:
    DataBox(int nrow = 0, int ncol = 0, QObject *pobj = 0);
    ~DataBox()
    {
        delete im;
    }

    void loadData(QString filePath);
    void loadData(QSqlQuery *base);
    void saveData();
    void output();

    QImage* getImage()
    {
        return im;
    }

    int numPopulations()
    {
        return cl_genes.size();
    }
    QString namePopulation(int i)
    {
        if (i >= 0 && i < cl_genes.size())
            return cl_genes[i].name;
        else
            qDebug() << "invalid population number";
        return "";
    }
    int numGenes()
    {
        return cl_genes[cl_activePop].genes.size();
    }
    int numMaxAlleles()
    {
        int max = cl_genes[cl_activePop].genes[0].alleles.size();
        for (int i = 1; i < cl_genes[cl_activePop].genes.size(); ++i)
            if (cl_genes[cl_activePop].genes[i].alleles.size() > max)
                max = cl_genes[cl_activePop].genes[i].alleles.size();
        return max;
    }
    int numAlleles(int i)
    {
        return cl_genes[cl_activePop].genes[i].alleles.size();
    }
    QString nameGene(int i)
    {
        if (i >= cl_genes[cl_activePop].genes.size())
            qDebug() << "no gene - " << i;
        return cl_genes[cl_activePop].genes[i].name;
    }
    QString allele(int i, int j)
    {
        if (j >= cl_genes[cl_activePop].genes[i].alleles.size())
            qDebug() << "allele. no allele - " << j << " in gene - " << i;
        return cl_genes[cl_activePop].genes[i].alleles[j].allele;
    }
    QString numH(int i, int j)
    {
        if (j >= cl_genes[cl_activePop].genes[i].alleles.size())
            qDebug() << "numH. no allele - " << j << " in gene - " << i;
        return cl_genes[cl_activePop].genes[i].alleles[j].numHealthy;
    }
    QString numI(int i, int j)
    {
        if (j >= cl_genes[cl_activePop].genes[i].alleles.size())
            qDebug() << "numI. no allele - " << j << " in gene - " << i;
        return cl_genes[cl_activePop].genes[i].alleles[j].numIll;
    }
    Gene getGene(int i)
    {
        return cl_genes[cl_activePop].genes[i];
    }
    int activePopulation()
    {
        return cl_activePop;
    }


    void setActivePopulation(int k)
    {
        if (k >= 0 && k < cl_genes.size())
            cl_activePop = k;
        else
            qDebug() << "no such population";
    }
    void setRowCount(int nrow)
    {
        cl_nrow = nrow;
    }
    int getRowCount()
    {
        return cl_nrow;
    }
    void setColumnCount(int ncol)
    {
        cl_ncolumn = ncol;
    }
    int getColumnCount()
    {
        return cl_ncolumn;
    }
    bool isEmpty()
    {
        return cl_genes.isEmpty();
    }

    void dataClear()
    {
        cl_nrow = 0;
        cl_ncolumn = 0;
        cl_data.clear();
    }

    void setDataToModel(DataForAlleles value)
    {
        cl_data.push_back(value);
    }
    void resetModel()
    {
        beginResetModel();
        endResetModel();
    }

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

};

#endif // DATABOX_H
