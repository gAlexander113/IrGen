#include "statplugin.h"

StatPlugin::StatPlugin()
{
}

QString StatPlugin::name() const
{
    return tr("Рассчитать коэффициенты");
}

QByteArray StatPlugin::statistics(DataBox *data)
{
    double Xi, res = 0;

    int numHealth = 0, numIll = 0; // R наблюдаемое
    int all;
    QModelIndex index1, index2;
    for (int i = 0; i < data->rowCount(QModelIndex()); ++i)
    {
        index1 = data->index(i, 2);
        numHealth += data->data(index1, Qt::DisplayRole).toInt();
        if (data->data(index1, Qt::DisplayRole).toInt() < 5)
            Xi = -1;
        index1 = data->index(i, 3);
        numIll += data->data(index1, Qt::DisplayRole).toInt();
        if (data->data(index1, Qt::DisplayRole).toInt() < 5)
            Xi = -1;
    }

    all = numHealth + numIll;

    double tmp1, tmp2; // R ожидаемое;

    for (int i = 0; i < data->rowCount(QModelIndex()); ++i)
    {
        index1 = data->index(i, 2);
        index2 = data->index(i, 3);
        tmp1 = (double) numHealth * (data->data(index1, Qt::DisplayRole).toInt() + data->data(index2, Qt::DisplayRole).toInt()) / all;
        tmp2 = (double) numIll * (data->data(index1, Qt::DisplayRole).toInt() + data->data(index2, Qt::DisplayRole).toInt()) / all;
        res += (data->data(index1, Qt::DisplayRole).toDouble() - tmp1 - 0.5) / tmp1 + (data->data(index2, Qt::DisplayRole).toDouble() - tmp2 - 0.5) / tmp2;
    }

    if (Xi != -1) Xi = res;

    double Phi;
    Phi = res / all;

    double C;
    C = Xi / (all * qSqrt(data->rowCount(QModelIndex()) - 1));


    /****************Отправляем данные******************/

    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    stream << Xi << Phi << C;

    return encodedData;
}
