#ifndef STATDIALOG_H
#define STATDIALOG_H

#include "main.h"
#include "ui_statwindow.h"

class StatDialog : public QWidget, Ui::StatWindow
{
    Q_OBJECT
private:
    QByteArray cl_data;
public:
    StatDialog(QWidget *pwgt = 0);
    void setData(QByteArray arr);
signals:
    void newData();
private slots:
    void slotSetData();
};

#endif // STATDIALOG_H
