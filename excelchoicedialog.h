#ifndef EXCELCHOICEDIALOG_H
#define EXCELCHOICEDIALOG_H

#include "main.h"
#include "ui_exceldialog.h"

class ExcelChoiceDialog : public QDialog, Ui::Form
{
    Q_OBJECT
public:
    ExcelChoiceDialog(QWidget *pwgt = 0);

    QString getFilePathH();
    QString getFilePathI();

private slots:
    void slotChooseH();
    void slotChooseI();
};

#endif // EXCELCHOICEDIALOG_H
