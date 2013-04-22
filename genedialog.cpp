#include "genedialog.h"

GeneDialog::GeneDialog(QWidget *pwgt)
    : QDialog(pwgt)
{
    cl_ui.setupUi(this);
    isMultipleSelection = false;
    setModal(true);

    connect(cl_ui.pbtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cl_ui.pbtnOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void GeneDialog::setGeneList(QStringList lst)
{
    if (!isMultipleSelection)
    {
        cl_ui.lstWgt->addItems(lst);
    }
    else
    {
        QListWidgetItem *pitem = 0;
        for (int i = 0; i < lst.size(); ++i)
        {
            pitem = new QListWidgetItem(lst.at(i), cl_ui.lstWgt);
            pitem->setFlags(pitem->flags() | Qt::ItemIsUserCheckable);
            pitem->setCheckState(Qt::Unchecked);
        }
    }
}

void GeneDialog::setMulitpleSelection(bool sel)
{
    isMultipleSelection = sel;
}

QString GeneDialog::getGene()
{
    QListWidgetItem *item = 0;
    QString str_return;
    if (!isMultipleSelection)
    {
        item = cl_ui.lstWgt->currentItem();
        str_return = item->text();
    }
    else
    {
        for (int i = 0; i < cl_ui.lstWgt->count(); ++i)
        {
            item = cl_ui.lstWgt->item(i);
            if (item->checkState() == Qt::Checked)
                str_return += item->text() + "-";
        }
        str_return.chop(1); // удаляем последний "-"
    }

    return str_return;
}
