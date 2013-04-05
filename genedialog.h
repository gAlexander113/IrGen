#ifndef GENEDIALOG_H
#define GENEDIALOG_H

#include "main.h"
#include "ui_genechoicedialog.h"

class GeneDialog : public QDialog
{
    Q_OBJECT
private:
    Ui::GeneChoiceDialog cl_ui;
public:
    GeneDialog(QWidget *pwgt = 0);

    void setGeneList(QStringList lst);
    QString getGene()
    {
        QListWidgetItem *pitem = cl_ui.lstWgt->currentItem();
        return pitem->text();
    }
};

#endif // GENEDIALOG_H
