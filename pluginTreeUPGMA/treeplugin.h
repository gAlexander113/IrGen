#ifndef TREEPLUGIN_H
#define TREEPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../IrGene_Qt_5_0_1/conttblinterface.h"
#include "graphicstextitem.h"
#include <QtSvg/QSvgGenerator>

struct Node
{
    QString name;
    Node *left;
    Node *right;
};

#define DD 150
#define HH 50


class TreePlugin : public QObject, ContInterface
{
    Q_OBJECT
    Q_INTERFACES(ContInterface)
    Q_PLUGIN_METADATA(IID "simple.com.IrGene_Qt_5_0_1.ContInterface" )
private:    
    double process_dist(QVector<Gene> genes1, QVector<Gene> genes2);
    void buildTree(QVector<Population> popPure, QImage *im);
    QStringList alleleIntersetion(QVector<Gene> tmpGenes);
public:
    TreePlugin();
    virtual QString name() const;
    virtual void tblResult(DataBox *data, QString nameGene);
};

bool lessThan(AlleleElement el1, AlleleElement el2);
double findMinDist(QVector<double> dist_matrix, int matrix_size, int &curInd1, int &curInd2);
QVector<double> recomputeMatrix(QVector<double> dist_matrix, int &matrix_size,
                                int curInd1, int curInd2, int numPops1, int numPops2);
int numpops(Node *node);
void writeTree(Node *node);
void paintTree(Node *node, QImage *image, QPointF pos, int d, QGraphicsScene *scene, int h);
int height(Node *node);

#endif // TREEPLUGIN_H
