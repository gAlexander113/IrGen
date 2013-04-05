#ifndef PROGRAM_H
#define PROGRAM_H

#include "main.h"
#include "ui_mainwindow.h"
#include "databox.h"
#include "genedialog.h"
#include "conttblinterface.h"
#include "statinterface.h"
#include "statdialog.h"

class Program : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
private:
    DataBox *cl_data;
    QLabel *cl_status;
    StatDialog *cl_dialog;

    void loadPlugins();
public:
    Program(QMainWindow *pwgt = 0);

private slots:
    void slotOpen();
    void slotSave();
    void slotLoadContTable();
    void slotLoadStatistics();
    void slotGroup();
};

#endif // PROGRAM_H
