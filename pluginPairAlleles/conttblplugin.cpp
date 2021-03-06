#include "conttblplugin.h"

ContTblPlugin::ContTblPlugin()
{
}

QString ContTblPlugin::name() const
{
    return QObject::tr("загрузить с учетом аллельной пары");
}

void ContTblPlugin::tblResult(DataBox *data, QString nameGene)
{
//    tbl->setColumnCount(8);

//    QStringList lst;
//    lst << QObject::tr("Наименование") << QObject::tr("Группа") << QObject::tr("Здоровые кол-во")
//        << QObject::tr("Больные кол-во") << QObject::tr("Здоровые частота")
//        << QObject::tr("Больные частота") << QObject::tr("Относит. риск(RR)")
//        << QObject::tr("95% доверит. инт.(RR)");

//    tbl->setHorizontalHeaderLabels(lst);
//    tbl->resizeColumnsToContents();

    data->dataClear();

    Gene curGene;

    int f = 0;
    for (int i = 0; i < data->numGenes(); ++i)
        if (nameGene == data->nameGene(i))
        {
            curGene = data->getGene(i);
            f = 1;
            break;
        }
    if (!f)
        qDebug() << "Error in pluginOneAllele: no such gene";

    QHash<QString, int> uniqHAlleles;
    QHash<QString, int> uniqIAlleles;
    int allH = 0, allI = 0;


    for (int i = 0; i < curGene.alleles.size(); ++i)
    {
        if (curGene.alleles[i].allele != "0,0")
        {
            QStringList strlst = curGene.alleles[i].allele.split(",");
            QString strReverse = strlst[1] + "," + strlst[0];
            if (uniqHAlleles.contains(curGene.alleles[i].allele))
            {
                int numH = uniqHAlleles.value(curGene.alleles[i].allele);
                numH += curGene.alleles[i].numHealthy.toInt();
                uniqHAlleles.insert(curGene.alleles[i].allele, numH);
            }
            else
                if (uniqHAlleles.contains(strReverse))
                {
                    int numH = uniqHAlleles.value(strReverse);
                    numH += curGene.alleles[i].numHealthy.toInt();
                    uniqHAlleles.insert(strReverse, numH);
                }
            else
                {
                    uniqHAlleles.insert(curGene.alleles[i].allele, curGene.alleles[i].numHealthy.toInt());
                }

            if (uniqIAlleles.contains(curGene.alleles[i].allele))
            {
                int numI = uniqIAlleles.value(curGene.alleles[i].allele);
                numI += curGene.alleles[i].numIll.toInt();
                uniqIAlleles.insert(curGene.alleles[i].allele, numI);
            }
            else
                if (uniqIAlleles.contains(strReverse))
                {
                    int numI = uniqIAlleles.value(strReverse);
                    numI += curGene.alleles[i].numIll.toInt();
                    uniqIAlleles.insert(strReverse, numI);
                }
            else
                {
                    uniqIAlleles.insert(curGene.alleles[i].allele, curGene.alleles[i].numIll.toInt());
                }
        }
    }


    QVector<QString> alleles;
    QVector<int> numH;
    QVector<int> numI;

    alleles = uniqHAlleles.keys().toVector();
    qSort(alleles.begin(), alleles.end());

    allH = 0;
    allI = 0;

    for (int i = 0; i < alleles.size(); ++i)
    {
        numH.push_back(uniqHAlleles.value(alleles[i]));
        numI.push_back(uniqIAlleles.value(alleles[i]));
        allH += numH.at(i);
        allI += numI.at(i);
    }

    qDebug() << "ALL" << allH << allI;

//    tbl->setRowCount(uniqHAlleles.size());
//    QTableWidgetItem *pitem;

    data->setColumnCount(8);
    data->setRowCount(uniqHAlleles.size());

    for (int i = 0; i < data->getRowCount(); ++i)
    {
        DataForAlleles var;
        for (int j = 0; j < data->getColumnCount(); ++j)
        {
            switch (j)
            {
            case 0: // наименование
//                pitem = new QTableWidgetItem(alleles[i]);
                var.name = alleles[i];
                break;
            case 1: // группа
//                pitem = new QTableWidgetItem("1");
                var.group = 1;
                break;
            case 2: // здоровые кол-во
//                pitem = new QTableWidgetItem(QString::number(numH[i]));
                var.numHealthy = numH[i];
                break;
            case 3: // больные кол-во
//                pitem = new QTableWidgetItem(QString::number(numI[i]));
                var.numIll = numI[i];
                break;
            case 4: // здоровые частота
//                pitem = new QTableWidgetItem(QString::number((double)numH[i] / allH));
                var.freakHealthy = (double)numH[i] / allH;
                break;
            case 5: // больные частота
//                pitem = new QTableWidgetItem(QString::number((double)numI[i] / allI));
                var.freakIll = (double)numI[i] / allI;
                break;
            case 6: // относ риск
                processRR(alleles[i], curGene);
//                pitem = new QTableWidgetItem(QString::number(RR));
                var.RR = RR;
                break;
            case 7: // 95% интервал
//                pitem = new QTableWidgetItem(confInt);
                var.RRInt = confInt;
                break;
            default:
                qDebug() << "no columns to show";
            }
//            pitem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//            tbl->setItem(i, j, pitem);
//            tbl->resizeRowsToContents();
            var.geneName = nameGene;
        }
        data->setDataToModel(var);
    }
    data->resetModel();
}

void ContTblPlugin::processRR(QString allele, Gene gene)
{
    double a, b, c, d; // коэфф таблицы сопряж.

    a = 0;
    b = 0;
    c = 0;
    d = 0;

    for (int i = 0; i < gene.alleles.size(); ++i)
    {
        QStringList lst = gene.alleles[i].allele.split(",");
        QString revAllele = lst[1] + "," + lst[0];
        if (allele == gene.alleles[i].allele)
        {
            a += gene.alleles[i].numHealthy.toInt();
            c += gene.alleles[i].numIll.toInt();
        }
        else
            if (allele == revAllele)
            {
                a += gene.alleles[i].numHealthy.toInt();
                c += gene.alleles[i].numIll.toInt();
            }
        else
            {
                b += gene.alleles[i].numHealthy.toInt();
                d += gene.alleles[i].numIll.toInt();
            }
    }
    qDebug() << allele << ": a = " << a << "d = " << d << "b =" << b << "c = " << c;
    if (a * b * c * d == 0)
    {
        a = a + 0.5;
        b = b + 0.5;
        c = c + 0.5;
        d = d + 0.5;
    }

    RR = (double) (b * c) / (a * d); // в статье сытина ошибка

    double left, right;
    double V;

    V = 1.96 * qSqrt(1.0 / a + 1.0 / b + 1.0 / c + 1.0 / d);
    left = qExp(qLn(RR) - V); // тоже не указано в статье, но в проге почемуто делаем e^(qLn(RR) - 1.96 * qSqrt(1.0 / a + 1.0 / b + 1.0 / c + 1.0 / d))
    right = qExp(qLn(RR) + V);

    confInt = "[" + QString::number(left) + " ; " + QString::number(right) + "]";
}
