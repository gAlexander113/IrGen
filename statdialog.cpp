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
    QDataStream stream(&cl_data, QIODevice::ReadOnly);

    QVector<double> tmparray;

    while (!stream.atEnd())
    {
        double tmp;
        stream >> tmp;
        tmparray.push_back(tmp);
    }
    // tmparray 0: Xi
    //          1: Phi
    //          2: C
    //          3: P
    //          4: Ka

    if (tmparray[0] == -1)
    {
        QString str = tr("Не работает");
        leXi->setText(str);
        lePhi->setText(str);
        leC->setText(str);
        leP->setText(str);
    }
    else
    {
        leXi->setText(QString::number(tmparray[0]));
        lePhi->setText(QString::number(tmparray[1]));
        leC->setText(QString::number(tmparray[2]));
        // и так далее
    }
}
