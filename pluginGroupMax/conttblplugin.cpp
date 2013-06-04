#include "conttblplugin.h"

ContTblPlugin::ContTblPlugin()
{
}


QString ContTblPlugin::name() const
{
    return tr("по max отношения шансов");
}

void ContTblPlugin::tblResult(DataBox *data, QString nameGene)
{
    Q_UNUSED(nameGene)

    QVector<QString> names;
    QVector<int> numH;
    QVector<int> numI;

    QModelIndex index1, index2;
    int max = 0;
    for (int i = 0; i < data->rowCount(QModelIndex()); ++i)
    {
        index1 = data->index(i, 1);
        if (data->data(index1, Qt::DisplayRole).toInt() > max)
            max = data->data(index1, Qt::DisplayRole).toInt();
    }

    names.resize(max);
    numH.resize(max);
    numI.resize(max);

    for (int i = 0; i < data->rowCount(QModelIndex()); ++i)
    {
        index1 = data->index(i, 1); //группа
        int group = data->data(index1, Qt::DisplayRole).toInt();
        index2 = data->index(i, 0); //имя аллели
        names[group - 1] += data->data(index2, Qt::DisplayRole).toString() + "; ";
        index2 = data->index(i, 2); //здор кол
        numH[group - 1] += data->data(index2, Qt::DisplayRole).toInt();
        index2 = data->index(i, 3); //боль кол
        numI[group - 1] += data->data(index2, Qt::DisplayRole).toInt();
    }
    qDebug() << "stage1 : " << names.size() << numH.size() << numI.size();
    /*******************************/
    /*      rewrite data           */
    /*******************************/

    data->dataClear();

    data->setColumnCount(8);
    data->setRowCount(names.size());

    int allH = 0, allI = 0;
    for (int i = 0; i < names.size(); ++i)
    {
        allH += numH[i];
        allI += numI[i];
    }

    for (int i = 0; i < data->getRowCount(); ++i)
    {
        DataForAlleles var;
        for (int j = 0; j < data->getColumnCount(); ++j)
        {
            switch (j)
            {
            case 0: // наименование
                var.name = names[i];
                break;
            case 1: // группа
                var.group = i + 1;
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
                processRR(names[i], names, numH, numI);
                var.RR = RR;
                break;
            case 7: // 95% интервал
                var.RRInt = confInt;
                break;
            default:
                qDebug() << "no columns to show";
            }
            var.geneName = nameGene;
        }
        data->setDataToModel(var);
    }
    data->resetModel();
}


void ContTblPlugin::processRR(QString allele, QVector<QString> names, QVector<int> numH, QVector<int> numI)
{
    double a, b, c, d; // коэфф таблицы сопряж.

    a = 0;
    b = 0;
    c = 0;
    d = 0;

    for (int i = 0; i < names.size(); ++i)
    {
        if (allele == names[i])
        {
            a += numH[i];
            c += numI[i];
        }
        else
            {
                b += numH[i];
                d += numI[i];
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
