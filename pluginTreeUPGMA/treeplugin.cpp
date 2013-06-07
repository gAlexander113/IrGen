#include "treeplugin.h"

TreePlugin::TreePlugin()
{
}

QString TreePlugin::name() const
{
    return QObject::tr("Метод невзвешенного попарного среднего(UPGMA)");
}

void TreePlugin::tblResult(DataBox *data, QString nameGene)
{
    QVector<Population> popPure; // здесь собираются нужные данные

    QStringList nameGenes = nameGene.split("-");

    for (int i = 0; i < data->numPopulations(); ++i) // выберем нужные гены из популяций
    {
        data->setActivePopulation(i);
        Population tmppop;
        tmppop.name = data->namePopulation(i);
        for (int j = 0; j < data->numGenes(); ++j)
            for (int k = 0; k < nameGenes.size(); ++k)
                if (data->nameGene(j) == nameGenes[k])
                {
                    tmppop.genes.push_back(data->getGene(j));
                    break;
                }
        popPure.push_back(tmppop);
    }

    //теперь нужно получить одинаковые аллели для тех же геннов в каждой популяции
    for (int i = 0; i < nameGenes.size(); ++i)
    {
        QVector<Gene> tmpGenes; // соберем набор одинаковых генов из каждой популяции
        for (int j = 0; j < popPure.size(); ++j)
            for (int k = 0; k < popPure[j].genes.size(); ++k)
                if (popPure[j].genes[k].name == nameGenes[i])
                {
                    tmpGenes.push_back(popPure[j].genes[k]);
                    break;
                }
        QStringList allelesInter = alleleIntersetion(tmpGenes); // пересечение множеств аллелей собранных геннов

        for (int j = 0; j < tmpGenes.size(); ++j) //удалим ненужные аллели
        {
            int k = 0;
            while (k < tmpGenes[j].alleles.size())
            {
                if (!allelesInter.contains(tmpGenes[j].alleles[k].allele))
                    tmpGenes[j].alleles.remove(k);
                else
                    k++;
            }
        }

        for (int j = 0; j < popPure.size(); ++ j) // вернем преобразованные гены на их законные места
            for (int k = 0; k < popPure[j].genes.size(); ++k)
                if (popPure[j].genes[k].name == nameGenes[i])
                {
                    popPure[j].genes[k] = tmpGenes[j];
                    break;
                }
    }

    Gene tmpGene;
    for (int i = 0; i < popPure.size(); ++i)
    {
        int length = popPure[i].genes.size();
        for (int k = 0; k < length - 1; ++k)
            for (int j = k + 1; j < length; ++j)
                if (popPure[i].genes[k].name > popPure[i].genes[j].name)
                {
                    tmpGene = popPure[i].genes[k];
                    popPure[i].genes[k] = popPure[i].genes[j];
                    popPure[i].genes[j] = tmpGene;
                }
    }

    for (int i = 0; i < popPure.size(); ++i)
        for (int j = 0; j < popPure[i].genes.size(); ++j)
            qSort(popPure[i].genes[j].alleles.begin(), popPure[i].genes[j].alleles.end(), lessThan);

    for (int i = 0; i < popPure.size(); ++i)
    {
        qDebug() << popPure[i].name;
        for (int j = 0; j < popPure[i].genes.size(); ++ j)
        {
            qDebug() << "  " << popPure[i].genes[j].name;
            for (int k = 0; k < popPure[i].genes[j].alleles.size(); ++k)
                qDebug() << popPure[i].genes[j].alleles[k].allele;
        }
    }

    buildTree(popPure, data->getImage());

}

QStringList TreePlugin::alleleIntersetion(QVector<Gene> tmpGenes)
{
    QStringList minDict;

    for (int i = 0; i < tmpGenes[0].alleles.size(); ++i)
    {
        minDict.push_back(tmpGenes[0].alleles[i].allele);
    }

    for (int k = 1; k < tmpGenes.size(); ++k)
    {
        int i = 0;
        while (i < minDict.size())
        {
            int f = 0;
            for (int j = 0; j < tmpGenes[k].alleles.size(); ++j)
                if (minDict[i] == tmpGenes[k].alleles[j].allele)
                {
                    f = 1;
                    break;
                }

            if (f)
                i++;
            else
                minDict.removeAt(i);
        }
    }

    return minDict;
}

bool lessThan(AlleleElement el1, AlleleElement el2)
{
    return el1.allele < el2.allele;
}

double TreePlugin::process_dist(QVector<Gene> genes1, QVector<Gene> genes2) // ген расстояние Нэя
{
    int numGenes = genes1.size();
    QVector<double> var1; // sum(a[i] * b[i]) для i гена
    QVector<double> var2; // sqrt(sum(a[i]^2) * sum(b[i]^2)) для i гена
    var1.resize(numGenes);
    var1.fill(0.0);
    var2.resize(numGenes);
    var2.fill(0.0);
    for (int i = 0; i < numGenes; ++i)
    {
        int all1 = 0, all2 = 0;
        for (int j = 0; j < genes1[i].alleles.size(); ++j)
        {
            all1 += genes1[i].alleles[j].numHealthy.toInt() + genes1[i].alleles[j].numIll.toInt();
            all2 += genes2[i].alleles[j].numHealthy.toInt() + genes2[i].alleles[j].numIll.toInt();
        }

        double sumAi = 0, sumBi = 0;
        for (int j = 0; j < genes1[i].alleles.size(); ++j)
        {
            double fr1 = (double)(genes1[i].alleles[j].numHealthy.toInt() + genes1[i].alleles[j].numIll.toInt()) / all1;
            double fr2 = (double)(genes2[i].alleles[j].numHealthy.toInt() + genes2[i].alleles[j].numIll.toInt()) / all2;
            var1[i] += fr1 * fr2;
            sumAi += fr1 * fr1;
            sumBi += fr2 * fr2;
        }
        var2[i] = qSqrt(sumAi * sumBi);
    }

    double mainVar1 = 0;
    double mainVar2 = 0;

    for (int i = 0; i < var1.size(); ++i)
    {
        mainVar1 += var1[i];
        mainVar2 += var2[i];
    }

    mainVar1 = (double) mainVar1 / numGenes;
    mainVar2 = (double) mainVar2 / numGenes;

    double I = mainVar1 / mainVar2;
    double D = -1 * qLn(I);

    return D;
}

void TreePlugin::buildTree(QVector<Population> popPure, QImage *im)
{
    QVector<double> dist_matrix; // строим матрицу расстояний
    int matrix_size = popPure.size();
    dist_matrix.resize(matrix_size * matrix_size);

    for (int i = 0; i < matrix_size; ++i)
        for (int j = 0; j < matrix_size; ++j)
            if (i != j)
                dist_matrix[i * matrix_size + j] = process_dist(popPure[i].genes, popPure[j].genes);
            else
                dist_matrix[i * matrix_size + j] = 0.0;

    for (int i = 0; i < matrix_size; ++i)
    {
        qDebug() << "new line";
        for (int j = 0; j < matrix_size; ++j)
            qDebug() << dist_matrix[i * matrix_size + j];
    }

    QList<Node *> lists; //список текущих узлов
    for (int i = 0; i < matrix_size; ++i)
    {
        Node *node = new Node;
        node->name = popPure[i].name;
        node->left = NULL;
        node->right = NULL;
        lists.push_back(node);
    }

    while (matrix_size > 1) //пока в матрице есть что объединять
    {
        int curInd1, curInd2; // номера стб и стр, которые будут объеденены
        double minDist = findMinDist(dist_matrix, matrix_size, curInd1, curInd2);
        int nump1 = 0, nump2 = 0;
        nump1 = numpops(lists[curInd1]);
        nump2 = numpops(lists[curInd2]);
        dist_matrix = recomputeMatrix(dist_matrix, matrix_size,
                                      curInd1, curInd2, nump1, nump2);
        for (int i = 0; i < matrix_size; ++i)
        {
            qDebug() << "new line";
            for (int j = 0; j < matrix_size; ++j)
                qDebug() << dist_matrix[i * matrix_size + j];
        }

        Node* newNode = new Node;
        newNode->name = QString::number(minDist);
        newNode->left = lists[curInd1];
        newNode->right = lists[curInd2];
        lists.removeAt(curInd2);
        lists.removeAt(curInd1);
        lists.insert(curInd1, newNode);
    }

    qDebug() << lists.size();
    writeTree(lists[0]);

    int nump = numpops(lists[0]);

    QGraphicsScene scene;

    ;
    int d = 150;
    int h = 160;
    qreal sc = 1;
    qDebug() << "scale koef = " << sc;
//    scene.setSceneRect(-(d * numpops(lists[0])) / 2, 0, (d * numpops(lists[0])) / 2, 100);

    scene.setSceneRect(0, 0, (numpops(lists[0]) + 2) * d, (height(lists[0]) + 2) * h);
    QPointF point(scene.width() / 2, h);
    paintTree(lists[0], im, point, (numpops(lists[0]) + 2) * d / 4, &scene, h);


    QSvgGenerator svgGen;

    svgGen.setFileName("Tree.svg");
    svgGen.setSize(QSize(500, 500));
    svgGen.setViewBox(QRect(0,0, 500, 500));

    QPainter painter(&svgGen);
    scene.render(&painter);
    painter.end();
//    im->save("tree.png");
    qDebug() << "num EL = " << numpops(lists[0]);
}

double findMinDist(QVector<double> dist_matrix, int matrix_size, int &curInd1, int &curInd2)
{
    double minDist = dist_matrix[0 * matrix_size + 1];
    curInd1 = 0;
    curInd2 = 1;
    for (int i = 0; i < matrix_size; ++i)
        for (int j = i + 1; j < matrix_size; ++j)
            if (dist_matrix[i * matrix_size + j] < minDist)
            {
                minDist = dist_matrix[i * matrix_size + j];
                curInd1 = i;
                curInd2 = j;
            }
    return minDist;
}

QVector<double> recomputeMatrix(QVector<double> dist_matrix, int &matrix_size,
                                int curInd1, int curInd2, int numPops1, int numPops2)
{
    int new_matrix_size = matrix_size - 1;
    QVector<double> new_dist_matrix;
    new_dist_matrix.resize(new_matrix_size * new_matrix_size);

    int old_i, old_j;
    for (int i = 0; i < new_matrix_size; ++i)
    {
        if (i >= curInd2)
            old_i = i + 1;
        else
            old_i = i;
        for (int j = i + 1; j < new_matrix_size; ++j)
        {
            if (j >= curInd2)
                old_j = j + 1;
            else
                old_j = j;

            if (i == curInd1)
                new_dist_matrix[i * new_matrix_size + j] =
                        (numPops1 * dist_matrix[curInd1 * matrix_size + old_j]
                        + numPops2 * dist_matrix[curInd2 * matrix_size + old_j])
                        / (numPops1 + numPops2);
            else
                if (j == curInd1)
                    new_dist_matrix[i * new_matrix_size + j] =
                            (numPops1 * dist_matrix[curInd1 * matrix_size + old_i]
                            + numPops2 * dist_matrix[curInd2 * matrix_size + old_i])
                            / (numPops1 + numPops2);
            else
                    new_dist_matrix[i * new_matrix_size + j] =
                            dist_matrix[old_i * matrix_size + old_j];
        }
    }

//    for (int i = 0; i < new_matrix_size; ++i)
//        for (int j = i; j < new_matrix_size; ++j)
//        {
//            if (i == j)
//                new_dist_matrix[i * new_matrix_size + j] = 0.0;
//            else
//                new_dist_matrix[j * new_matrix_size + i] = new_dist_matrix[i * new_matrix_size + j];
//        }

    matrix_size = new_matrix_size;

    return new_dist_matrix;
}

int numpops(Node *node)
{
    int sum = 0;
    if (node->left == NULL && node->right == NULL)
    {
        return 1;
    }

    sum = numpops(node->left) + numpops(node->right);

    return sum;
}

void writeTree(Node *node)
{
    if (node == NULL)
    {
        qDebug() << "list";
        return;
    }
    else
        qDebug() << node->name;

    writeTree(node->left);
    writeTree(node->right);
}

void paintTree(Node *node, QImage *image, QPointF pos, int d, QGraphicsScene *scene, int h)
{
    if (node->left == NULL && node->right == NULL)
    {
        QFont font("Arial", 28, QFont::Normal);
        font.setStyleHint(QFont::TypeWriter);
        QGraphicsTextItem *ptext = scene->addText(node->name, font);

        ptext->setPos(pos.x(), pos.y());
        ptext->setRotation(-90);
        ptext->setPos(ptext->pos().x() - 55, ptext->pos().y());
//        ptext->setPos(ptext->pos().x(), ptext->pos().y() + 50);
//        ptext->setScale(sc);
        return;

    }

    QPointF pright(pos.x() + d, pos.y() + h);
    QPointF pleft(pos.x() - d, pos.y() + h);

    scene->addLine(pos.x() - d, pos.y(), pos.x() + d, pos.y()); // top line
    scene->addLine(pos.x() - d, pos.y(), pos.x() - d, pos.y() + h); // left line
    scene->addLine(pos.x() + d, pos.y(), pos.x() + d, pos.y() + h); // right line

    paintTree(node->left, image, pleft, d / 2, scene, h);
    paintTree(node->right, image, pright, d / 2, scene, h);

}

//void paintTree(Node *node, QImage *image, QPointF pos, double d, QGraphicsScene *scene, double sc)
//{
//    if (node->left == NULL && node->right == NULL)
//    {
//        QGraphicsTextItem *ptext = scene->addText(node->name, QFont("Arial", 8, QFont::Normal));

//        ptext->setPos(pos.x() - d / 4.0, pos.y() + 1);
//        ptext->setScale(sc);
//        return;
//    }
//    QPointF leftp, rightp;
//    leftp.setX(pos.x() - d / 4.0);
//    leftp.setY(pos.y() + node->name.toDouble() * 100);
//    rightp.setX(pos.x() + d / 4.0);
//    rightp.setY(pos.y() + node->name.toDouble() * 100);

//    QGraphicsLineItem *plineItem = scene->addLine(QLine(pos.x() - d / 4.0, pos.y(), pos.x() + d / 4.0, pos.y()), QPen(Qt::black, 1));
//    plineItem = scene->addLine(pos.x() - d / 4.0, pos.y(), pos.x() - d / 4.0, pos.y() + node->name.toDouble() * 100, QPen(Qt::black, 1));
//    plineItem = scene->addLine(pos.x() + d / 4.0, pos.y(), pos.x() + d / 4.0, pos.y() + node->name.toDouble() * 100, QPen(Qt::black, 1));

//    paintTree(node->left, image, leftp, d / 2.0, scene, sc);
//    paintTree(node->right, image, rightp, d / 2.0, scene, sc);
//}




int height(Node *node)
{
    int h1 = 0, h2 = 0;
    if (node == NULL)
        return 0;

    h1 = height(node->left);
    h2 = height(node->right);
    return h1 > h2 ? (h1 + 1) : (h2 + 1);
}
