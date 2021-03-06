#ifndef PROGRAM_H
#define PROGRAM_H

#include "main.h"
#include "ui_mainwindow.h"
#include "databox.h"
#include "genedialog.h"
#include "conttblinterface.h"
#include "statinterface.h"
#include "statdialog.h"
#include "graphics.h"
#include "excelchoicedialog.h"
#include "databaseinterface.h"
#include "logindialogcl.h"

class Program : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
private:
    DataBox *cl_data;
    QLabel *cl_status;
    StatDialog *cl_dialog;
    Graphics *cl_graphics;

    void loadPlugins();
    void showData();
    QVector<QString> geneNameList(QVector< QVector<QString> > nglst);
public:
    Program(QMainWindow *pwgt = 0);
    ~Program();

private slots:
    void slotOpen();
    void slotOpenDB();
    void slotImportData();
    void slotSave();
    void slotLoadContTable();
    void slotLoadStatistics();
    void slotGroup();
    void slotBuildTree();
    void slotDrawGraph();    
};

#endif // PROGRAM_H
