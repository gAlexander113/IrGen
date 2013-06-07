#include "excelchoicedialog.h"

ExcelChoiceDialog::ExcelChoiceDialog(QWidget *pwgt)
    :QDialog(pwgt)
{
    setupUi(this);

    this->setModal(true);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    connect(btnHealthy, SIGNAL(clicked()), this, SLOT(slotChooseH()));
    connect(btnIll, SIGNAL(clicked()), this, SLOT(slotChooseI()));
}

QString ExcelChoiceDialog::getFilePathH()
{
    return lineEdit->text();
}

QString ExcelChoiceDialog::getFilePathI()
{
    return lineEdit_2->text();
}

void ExcelChoiceDialog::slotChooseH()
{
    QString str = QFileDialog::getOpenFileName(0, tr("Открыть файл"), "", "*.xls");
    lineEdit->setText(str);
}

void ExcelChoiceDialog::slotChooseI()
{
    QString str = QFileDialog::getOpenFileName(0, tr("Открыть файл"), "", "*.xls");
    lineEdit_2->setText(str);
}
