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

    Gene curGene1, curGene2;

    QStringList lst = nameGene.split("-");

    for (int i = 0; i < data->numGenes(); ++i)
        if (lst[0] == data->nameGene(i))
        {
            curGene1 = data->getGene(i);
            break;
        }
    for (int i =  0; i < data->numGenes(); ++i)
        if (lst[1] == data->nameGene(i))
        {
            curGene2 = data->getGene(i);
            break;
        }

    QHash<QString, int> uniqHAlleles;
    QHash<QString, int> uniqIAlleles;
    int allH = 0, allI = 0;

    int maxAlleles = (curGene1.alleles.size() > curGene2.alleles.size()) ? curGene1.alleles.size()
                                                                         : curGene2.alleles.size();

    for (int i = 0; i < maxAlleles; ++i)
    {
        if (curGene1.alleles[i].allele != "0,0" && curGene2.alleles[i].allele != "0,0")
        {
            QStringList strlst1, strlst2;
            strlst1 = curGene1.alleles[i].allele.split(",");
            strlst2 = curGene2.alleles[i].allele.split(",");
            QString al11 = strlst1[0] + "," + strlst2[0];
            QString al22 = strlst1[1] + "," + strlst2[1];

            if (uniqHAlleles.contains(al11))
            {
                int numH = uniqHAlleles.value(al11);
                numH += curGene1.alleles[i].numHealthy.toInt(); // значения кол-ва здоровых у гена1 и гена2 одинаково
                uniqHAlleles.insert(al11, numH);
            }
            else
            {
                uniqHAlleles.insert(al11, curGene1.alleles[i].numHealthy.toInt());
            }

            if (uniqHAlleles.contains(al22))
            {
                int numH = uniqHAlleles.value(al22);
                numH += curGene1.alleles[i].numHealthy.toInt();
                uniqHAlleles.insert(al22, numH);
            }
            else
            {
                uniqHAlleles.insert(al22, curGene1.alleles[i].numHealthy.toInt());
            }

            if (uniqIAlleles.contains(al11))
            {
                int numI = uniqIAlleles.value(al11);
                numI += curGene1.alleles[i].numIll.toInt();
                uniqIAlleles.insert(al11, numI);
            }
            else
            {
                uniqIAlleles.insert(al11, curGene1.alleles[i].numIll.toInt());
            }

            if (uniqIAlleles.contains(al22))
            {
                int numI = uniqIAlleles.value(al22);
                numI += curGene1.alleles[i].numIll.toInt();
                uniqIAlleles.insert(al22, numI);
            }
            else
            {
                uniqIAlleles.insert(al22, curGene1.alleles[i].numIll.toInt());
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
                processRR(alleles[i], curGene1, curGene2);
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


void ContTblPlugin::processRR(QString allele, Gene gene1, Gene gene2)
{
    double a, b, c, d; // коэфф таблицы сопряж.

    a = 0;
    b = 0;
    c = 0;
    d = 0;

    int maxNumAl = (gene1.alleles.size() > gene2.alleles.size()) ? gene1.alleles.size()
                                                                 : gene2.alleles.size();

    for (int i = 0; i < maxNumAl; ++i)
    {
        if (gene1.alleles[i].allele != "0,0" && gene2.alleles[i].allele != "0,0")
        {
            QStringList strlst1, strlst2;
            strlst1 = gene1.alleles[i].allele.split(",");
            strlst2 = gene2.alleles[i].allele.split(",");
            QString al11 = strlst1[0] + "," + strlst2[0];
            QString al22 = strlst1[0] + "," + strlst2[0];

            if (allele == al11 || allele == al22)
            {
                a += gene1.alleles[i].numHealthy.toDouble();
                c += gene1.alleles[i].numIll.toDouble();
            }
            else
            {
                b += gene1.alleles[i].numHealthy.toDouble();
                d += gene1.alleles[i].numIll.toDouble();
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
