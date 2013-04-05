#include "genedialog.h"

GeneDialog::GeneDialog(QWidget *pwgt)
    : QDialog(pwgt)
{
    cl_ui.setupUi(this);

    setModal(true);

    connect(cl_ui.pbtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cl_ui.pbtnOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void GeneDialog::setGeneList(QStringList lst)
{
    cl_ui.lstWgt->addItems(lst);
}
