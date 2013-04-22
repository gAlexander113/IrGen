#include "conttblplugin.h"

ContTblPlugin::ContTblPlugin()
{
}

QString ContTblPlugin::name() const
{
    return QObject::tr("загрузить с учетом гаплотипа");
}

void ContTblPlugin::tblResult(DataBox *data, QString nameGene) // nameGene: "name-name"
{
    data->dataClear();

    QList<Gene> curGenes;

    QStringList lst = nameGene.split("-");

    for (int i = 0; i < data->numGenes(); ++i)
    {
        for (int j = 0; j < lst.size(); ++j)
        {
            if (lst.at(j) == data->nameGene(i))
            {
                curGenes.push_back(data->getGene(i));
                lst.removeAt(j);
                break;
            }
        }
    }

    QHash<QString, int> uniqHAlleles;
    QHash<QString, int> uniqIAlleles;
    int allH = 0, allI = 0;

    int maxAlleles = curGenes.at(0).alleles.size();
    for (int i = 1; i < curGenes.size(); ++i)
        if (curGenes.at(i).alleles.size() > maxAlleles)
            maxAlleles = curGenes.at(i).alleles.size();


    for (int i = 0; i < maxAlleles; ++i)
    {              
        if (!zeroAlleles(curGenes, i))
        {
            QString al11, al22;

            for (int k = 0; k < curGenes.size(); ++k)
            {
                QStringList strlst;
                strlst = curGenes[k].alleles[i].allele.split(",");
                al11 += strlst[0] + ",";
                al22 += strlst[1] + ",";
            }
            al11.chop(1);
            al22.chop(1);

            if (uniqHAlleles.contains(al11))
            {
                int numH = uniqHAlleles.value(al11);
                numH += curGenes[0].alleles[i].numHealthy.toInt(); // значения кол-ва здоровых у гена1 и гена2 одинаково
                uniqHAlleles.insert(al11, numH);
            }
            else
            {
                uniqHAlleles.insert(al11, curGenes[0].alleles[i].numHealthy.toInt());
            }

            if (uniqHAlleles.contains(al22))
            {
                int numH = uniqHAlleles.value(al22);
                numH += curGenes[0].alleles[i].numHealthy.toInt();
                uniqHAlleles.insert(al22, numH);
            }
            else
            {
                uniqHAlleles.insert(al22, curGenes[0].alleles[i].numHealthy.toInt());
            }

            if (uniqIAlleles.contains(al11))
            {
                int numI = uniqIAlleles.value(al11);
                numI += curGenes[0].alleles[i].numIll.toInt();
                uniqIAlleles.insert(al11, numI);
            }
            else
            {
                uniqIAlleles.insert(al11, curGenes[0].alleles[i].numIll.toInt());
            }

            if (uniqIAlleles.contains(al22))
            {
                int numI = uniqIAlleles.value(al22);
                numI += curGenes[0].alleles[i].numIll.toInt();
                uniqIAlleles.insert(al22, numI);
            }
            else
            {
                uniqIAlleles.insert(al22, curGenes[0].alleles[i].numIll.toInt());
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
                var.name = alleles[i];
                break;
            case 1: // группа
                var.group = 1;
                break;
            case 2: // здоровые кол-во
                var.numHealthy = numH[i];
                break;
            case 3: // больные кол-во
                var.numIll = numI[i];
                break;
            case 4: // здоровые частота
                var.freakHealthy = (double)numH[i] / allH;
                break;
            case 5: // больные частота
                var.freakIll = (double)numI[i] / allI;
                break;
            case 6: // относ риск
                processRR(alleles[i], curGenes);
                var.RR = RR;
                break;
            case 7: // 95% интервал
                var.RRInt = confInt;
                break;
            default:
                qDebug() << "no columns to show";
            }
        }
        data->setDataToModel(var);
    }
    data->resetModel();
}


void ContTblPlugin::processRR(QString allele, QList<Gene> genes)
{
    double a, b, c, d; // коэфф таблицы сопряж.

    a = 0;
    b = 0;
    c = 0;
    d = 0;

    int maxNumAl = genes[0].alleles.size();
    for (int i = 1; i < genes.size(); ++i)
        if (genes[i].alleles.size() > maxNumAl)
            maxNumAl = genes[i].alleles.size();

    for (int i = 0; i < maxNumAl; ++i)
    {
        if (!zeroAlleles(genes, i))
        {
            QString al11, al22;

            for (int k = 0; k < genes.size(); ++k)
            {
                QStringList strlst;
                strlst = genes[k].alleles[i].allele.split(",");
                al11 += strlst[0] + ",";
                al22 += strlst[1] + ",";
            }
            al11.chop(1);
            al22.chop(1);

            if (allele == al11 || allele == al22)
            {
                a += genes[0].alleles[i].numHealthy.toDouble();
                c += genes[0].alleles[i].numIll.toDouble();
            }
            else
            {
                b += genes[0].alleles[i].numHealthy.toDouble();
                d += genes[0].alleles[i].numIll.toDouble();
            }
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

bool ContTblPlugin::zeroAlleles(QList<Gene> genes, int i)
{
    for (int j = 0; j < genes.size(); ++j)
        if (genes.at(j).alleles[i].allele == "0,0")
            return true;
    return false;
}
