#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QtWidgets>

struct AlleleElement
{
    QString allele; // может быть либо одна аллель, либо пара, либо 2 пары
    QString numIll;
    QString numHealthy;
};

struct Gene
{
    QString name;
    QVector<AlleleElement> alleles;
};

struct DataForAlleles
{
    QString geneName;
    QString name;
    int group;
    int numIll;
    int numHealthy;
    double freakIll;
    double freakHealthy;
    double RR;
    QString RRInt; // format: "[double;double]"
};

struct Population
{
    QString name;
    QVector<Gene> genes;
};


#endif // MAIN_H
