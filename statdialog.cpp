#include "statdialog.h"

StatDialog::StatDialog(QWidget *pwgt)
    : QWidget(pwgt)
{
    setupUi(this);

    connect(pbtnOk, SIGNAL(clicked()), this, SLOT(close()));
    connect(this, SIGNAL(newData()), SLOT(slotSetData()));
}

void StatDialog::setData(QByteArray arr)
{
    cl_data = arr;
    emit newData();
}


void StatDialog::slotSetData()
{
    listWidget->clear();

    QDataStream stream(&cl_data, QIODevice::ReadOnly);

    QVector<QString> tmparray;

    while (!stream.atEnd())
    {
        QString tmp;
        stream >> tmp;
        tmparray.push_back(tmp);
    }
    // tmparray 0: Xi
    //          1: Phi
    //          2: C
    //          3: numRows : needed for Ka
    //          4: Ka

    QListWidgetItem *pitem = 0;
    if (tmparray[0].contains("-1"))
    {
        QString str = tr("Не работает");
        for (int i = 0; i < tmparray.size() - 1; ++i)
        {
            if (tmparray[i].contains("-"))
            {
                QStringList tmplist = tmparray[i].split("-");
                pitem = new QListWidgetItem(tmplist[0] + "- " + str, listWidget);
            }
        }
    }
    else
    {
        for (int i = 0; i < tmparray.size() - 2; ++i)
        {
            if (tmparray[i].contains("-"))
            {
                pitem = new QListWidgetItem(tmparray[i], listWidget);
            }
        }
    }
    qDebug() << "statDialog " << tmparray[4] << tmparray.size();
    if (tmparray[3].toInt() == 2)
        pitem = new QListWidgetItem(tmparray[4], listWidget);
    else
        pitem = new QListWidgetItem("Ka     - ", listWidget);
}
