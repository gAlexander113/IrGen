#include "program.h"

Program::Program(QMainWindow *pwgt)
    : QMainWindow(pwgt)
{
    setupUi(this);

    cl_data = new DataBox(0, 8);
    cl_graphics = new Graphics();

    tblViewResult->setModel(cl_data);
    tblViewResult->resizeColumnsToContents();

    cl_dialog = new StatDialog();

    loadPlugins();

    connect(btnExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(btnOpen, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(btnSave, SIGNAL(triggered()), this, SLOT(slotSave()));
    connect(btnImport, SIGNAL(triggered()), this, SLOT(slotImportData()));
    connect(action_GroupGom, SIGNAL(triggered()), this, SLOT(slotGroup()));
    connect(action, SIGNAL(triggered()), this, SLOT(slotDrawGraph()));


    cl_status = new QLabel;
    statBar->addWidget(cl_status);

}

Program::~Program()
{
    delete cl_data;
    delete cl_dialog;
    delete cl_status;
    delete cl_graphics;
}

void Program::loadPlugins()
{
    QDir dir(QApplication::applicationDirPath());

    if (!dir.cd("plugins"))
    {
        QMessageBox::critical(0, "", "plugins directory does not exist");
        return;
    }

    foreach (QString strFileName, dir.entryList(QDir::Files))
    {
        QPluginLoader loader(dir.absoluteFilePath(strFileName));

        QObject *pobj = loader.instance();
        if (!pobj)
        {
            qDebug() << loader.errorString();
            return;
        }

        ContInterface *pInterface = qobject_cast<ContInterface*> (pobj);
        if (pInterface)
        {
            QString str = pInterface->name();

            /***************************/
            /*     Group tools          */
            /***************************/

            if (str == tr("по гомозиготности") || str == tr("по max отношения шансов"))
            {
                QAction *pact = new QAction(str, pobj);
                connect(pact, SIGNAL(triggered()), this, SLOT(slotGroup()));
                menu_4->addAction(pact);
            }
            else
            {
                /************************/
                /* Contingency tables   */
                /************************/
                QAction *pact = new QAction(str, pobj);
                connect(pact, SIGNAL(triggered()), this, SLOT(slotLoadContTable()));
                menu_2->addAction(pact);
            }
        }
        else
            qDebug() << "this is not ContInterface";

        /*******************************/
        /*          Statistics         */
        /*******************************/
        StatInterface *pStatInt = qobject_cast<StatInterface*> (pobj);
        if (pStatInt)
        {
            QString str = pStatInt->name();
            QAction *pact = new QAction(str, pobj);
            connect(pact, SIGNAL(triggered()), this, SLOT(slotLoadStatistics()));
            menu_3->addAction(pact);
        }
        else
            qDebug() << "this is not StatInterface";
    }
}

void Program::showData()
{
    treeInfo->clear();
    QStringList lst;
    lst << tr("Данные") << tr("здоровых") << tr("больных");
    treeInfo->setHeaderLabels(lst);

    QTreeWidgetItem *pmainItem = new QTreeWidgetItem(treeInfo);
    pmainItem->setText(0, "Гены");

    QTreeWidgetItem *pgeneitem = 0, *pitem = 0;
    for (int i = 0; i < cl_data->numGenes(); ++i)
    {
        pgeneitem = new QTreeWidgetItem(pmainItem);
        pgeneitem->setText(0, cl_data->nameGene(i));
        for (int j = 0; j < cl_data->numAlleles(i); ++j)
        {
            pitem = new QTreeWidgetItem(pgeneitem);
            pitem->setText(0, cl_data->allele(i, j));
            pitem->setText(1, cl_data->numH(i, j));
            pitem->setText(2, cl_data->numI(i, j));
        }
    }
    treeInfo->setColumnWidth(0, 100);
    treeInfo->resizeColumnToContents(1);
    treeInfo->resizeColumnToContents(2);
}

void Program::slotOpen() // может отображение необработанных данных стоит убрать
{
    QString filePath = QFileDialog::getOpenFileName(0, QObject::tr("Открыть файл"), "", "*.xml");

    cl_data->loadData(filePath);
//    cl_data->output();

    showData();

    cl_status->setText(tr("Данные загруженны."));
}

void Program::slotImportData()
{
    ExcelChoiceDialog *dialog = new ExcelChoiceDialog;
    if (dialog->exec() == QDialog::Accepted)
    {
        QString filePathH = dialog->getFilePathH();
        QString filePathI = dialog->getFilePathI();

        QString str = QApplication::applicationDirPath();
        QString fileRes = str + "/result.xml";
        str += "/ConverterXLStoXML.exe";
        qDebug() << "program path = " << str << " " << filePathH << " " << filePathI << " " << fileRes;

        QProcess process;
        process.start(str + " " + filePathH + " " + filePathI + " " + fileRes);
        process.waitForFinished();

        cl_data->loadData(fileRes);
        showData();
    }
    delete dialog;

}

void Program::slotSave()
{
    cl_data->saveData();
}

void Program::slotLoadContTable()
{
    if (cl_data->isEmpty())
    {
        QMessageBox::information(0, tr("Ошибка"), tr("Загрузите данные!"));
        return;
    }

    QAction *pact = qobject_cast<QAction*> (sender());

    ContInterface *pInterface = qobject_cast<ContInterface*> (pact->parent());

    QStringList lst;
    for (int i = 0; i < cl_data->numGenes(); ++i)
        lst.append(cl_data->nameGene(i));

    if (pInterface->name() == QObject::tr("загрузить с учетом 1 аллели"))
    {
        GeneDialog *dialog = new GeneDialog;
        dialog->setGeneList(lst);
        if (dialog->exec() == QDialog::Accepted)
        {
            pInterface->tblResult(cl_data, dialog->getGene());
//            qDebug() << "row col " << cl_data->getRowCount() << cl_data->getColumnCount();
            tblViewResult->resizeColumnsToContents();
        }
        delete dialog;
    }

    if (pInterface->name() == QObject::tr("загрузить с учетом аллельной пары"))
    {
        GeneDialog *dialog = new GeneDialog;
        dialog->setGeneList(lst);
        if (dialog->exec() == QDialog::Accepted)
        {
            pInterface->tblResult(cl_data, dialog->getGene());
            tblViewResult->resizeColumnsToContents();
        }
        delete dialog;
    }

    if (pInterface->name() == QObject::tr("загрузить с учетом гаплотипа"))
    {
        GeneDialog *dialog = new GeneDialog;
        dialog->setMulitpleSelection(true);
        dialog->setGeneList(lst);
        if (dialog->exec() == QDialog::Accepted)
        {
            QString str = dialog->getGene();
            QStringList strlst = str.split("-");
            if (strlst.size() == 1)
            {
                QMessageBox::information(0, tr("Ошибка"), tr("Выберите несколько генов!"));
                return;
            }
            pInterface->tblResult(cl_data, str);
            tblViewResult->resizeColumnsToContents();
        }
        delete dialog;
    }

    QString str;
    int health = 0, ill = 0;
    for (int i = 0; i < cl_data->rowCount(QModelIndex()); ++i)
    {
        QModelIndex index = cl_data->index(i, 2);
        health += cl_data->data(index, Qt::DisplayRole).toInt();
        index = cl_data->index(i, 3);
        ill += cl_data->data(index, Qt::DisplayRole).toInt();
    }
    str = tr("Здоровые: ") + QString::number(health) + tr(" Больные: ") + QString::number(ill)
            + tr(" Всего: ") + QString::number(health + ill);
    cl_status->setText(str);
}

void Program::slotLoadStatistics()
{
    QAction *pact = qobject_cast<QAction*> (sender());

    StatInterface *pInterface = qobject_cast<StatInterface*> (pact->parent());

    cl_dialog->setData(pInterface->statistics(cl_data));
    cl_dialog->show();
}

void Program::slotGroup()
{
    if (cl_data->isEmpty())
    {
        QMessageBox::information(0, tr("Ошибка"), tr("Загрузите данные!"));
        return;
    }

    QModelIndex index;
    QString str;
    QStringList strList;
    QAction *pact = qobject_cast<QAction*> (sender());

    ContInterface *pInterface = qobject_cast<ContInterface*> (pact->parent());    

    if (pact->text() == tr("по гомозиготности"))
    {
        /*******************************/
        /*Проверка, можно ли сгруппировать*/
        /******************************/
        index = cl_data->index(0, 0);
        if (cl_data->data(index, Qt::DisplayRole).toString().size() != 3) // если не пара аллелей типа "1,1" :)
        {
            QMessageBox::information(0, tr("Предупреждение"), tr("не могу сгрупировать по гомозиготности."));
            return;
        }
        /****************************/
        /*      Проверка пройдена    */
        /***************************/
        for (int i = 0; i < cl_data->rowCount(QModelIndex()); ++i)
        {
            index = cl_data->index(i, 0); //Смотрим на аллель
            str = cl_data->data(index, Qt::DisplayRole).toString();
            if (str.size() < 2)
            {
                qDebug() << "group only for pair alleles";
                return;
            }

            strList = str.split(",");
            index = cl_data->index(i, 1);
            cl_data->setData(index, 1, Qt::EditRole);
            if (strList[0] == strList[1])
            {
                cl_data->setData(index, 2, Qt::EditRole);
            }
        }

        pInterface->tblResult(cl_data, "");
        tblViewResult->resizeColumnsToContents();
    }
    else
        if (pact->text() == tr("по max отношения шансов"))
        {
            double max = 0;
            for (int i = 0; i < cl_data->rowCount(QModelIndex()); ++i)
            {
                index = cl_data->index(i, 6);
                if (cl_data->data(index, Qt::DisplayRole).toDouble() > max)
                    max = cl_data->data(index, Qt::DisplayRole).toDouble();
            }

            for (int i = 0; i < cl_data->rowCount(QModelIndex()); ++i)
            {
                index = cl_data->index(i, 6);

                if (cl_data->data(index, Qt::DisplayRole).toDouble() == max)
                {
                    index = cl_data->index(i, 1);
                    cl_data->setData(index, 2, Qt::EditRole);
                }
            }
            pInterface->tblResult(cl_data, "");
            tblViewResult->resizeColumnsToContents();
        }
}

void Program::slotDrawGraph()
{
    cl_graphics->drawGraphFrequency(cl_data);
    cl_graphics->show();
}


