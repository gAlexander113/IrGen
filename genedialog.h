#ifndef GENEDIALOG_H
#define GENEDIALOG_H

#include "main.h"
#include "ui_genechoicedialog.h"

class GeneDialog : public QDialog
{
    Q_OBJECT
private:
    Ui::GeneChoiceDialog cl_ui;
    bool isMultipleSelection;
public:
    GeneDialog(QWidget *pwgt = 0);

    void setGeneList(QStringList lst);
    void setMulitpleSelection(bool sel);
    QString getGene();

};

#endif // GENEDIALOG_H
