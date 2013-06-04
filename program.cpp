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
//    connect(actBuidTree, SIGNAL(triggered()), this, SLOT(slotBuildTree()));

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
                if (str == tr("Метод невзвешенного попарного среднего(UPGMA)"))
                {
                    QAction *pact = new QAction(str, pobj);
                    connect(pact, SIGNAL(triggered()), this, SLOT(slotBuildTree()));
                    menu_5->addAction(pact);
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

        DataBaseInterface *pdbInt = qobject_cast<DataBaseInterface*> (pobj);
        if (pdbInt)
        {
            QString str = pdbInt->name();
            QAction *pact = new QAction(str, pobj);
            connect(pact, SIGNAL(triggered()), this, SLOT(slotOpenDB()));
            menuDataBases->addAction(pact);
        }
        else
            qDebug() << "this is not DBInt";
    }
}

void Program::showData()
{
    treeInfo->clear();
    QStringList lst;
    lst << tr("Данные") << tr("здоровых") << tr("больных");
    treeInfo->setHeaderLabels(lst);

    QTreeWidgetItem *pmainItem = new QTreeWidgetItem(treeInfo);
    pmainItem->setText(0, "Популяции");

    QTreeWidgetItem *ppopitem = 0, *pgeneitem = 0, *pitem = 0;
    for (int i = 0; i < cl_data->numPopulations(); ++i)
    {
        ppopitem = new QTreeWidgetItem(pmainItem);
        ppopitem->setText(0, cl_data->namePopulation(i));
        cl_data->setActivePopulation(i);
        for (int k = 0; k < cl_data->numGenes(); ++k)
        {
            pgeneitem = new QTreeWidgetItem(ppopitem);
            pgeneitem->setText(0, cl_data->nameGene(k));
            for (int j = 0; j < cl_data->numAlleles(k); ++j)
            {
                pitem = new QTreeWidgetItem(pgeneitem);
                pitem->setText(0, cl_data->allele(k, j));
                pitem->setText(1, cl_data->numH(k, j));
                pitem->setText(2, cl_data->numI(k, j));
            }
        }
    }
    treeInfo->setColumnWidth(0, 130);
    treeInfo->resizeColumnToContents(1);
    treeInfo->resizeColumnToContents(2);
}

void Program::slotOpen()
{
    QString filePath = QFileDialog::getOpenFileName(0, QObject::tr("Открыть файл"), "", "*.xml");

    cl_data->loadData(filePath);
    cl_data->output();

    showData();

    cl_status->setText(tr("Данные загруженны."));
}

void Program::slotOpenDB()
{
    QAction *pact = qobject_cast<QAction*> (sender());
    DataBaseInterface *pdbInt = qobject_cast<DataBaseInterface*>(pact->parent());

    LoginDialogCl *dialog = new LoginDialogCl;
    if (dialog->exec() == QDialog::Accepted)
    {
        pdbInt->setUser(dialog->getname(), dialog->getpass());
        pdbInt->getQuery();
    }
    delete dialog;

//    cl_data->loadData(query);
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

    QTreeWidgetItem *pitem = treeInfo->currentItem();
    if (!treeInfo->selectionModel()->hasSelection() || (pitem->parent() == NULL || pitem->parent()->text(0) != "Популяции"))
    {
        QMessageBox::information(0, tr("Ошибка"), tr("Выберите популяцию для анализа!"));
        return;
    }
    else
    {
        QTreeWidgetItem *pBatiaItem = pitem->parent();
        for (int i = 0; i < pBatiaItem->childCount(); ++i)
            if (pitem->text(0) == pBatiaItem->child(i)->text(0))
            {
                cl_data->setActivePopulation(i);
                break;
            }
        qDebug() << "текущая популяция" << cl_data->activePopulation();
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
    cl_graphics->resize(500, 600);
    cl_graphics->show();
}

void Program::slotBuildTree()
{
    QAction *pact = qobject_cast<QAction*>(sender());

    ContInterface *pI = qobject_cast<ContInterface*>(pact->parent());

    int curSelectedPop = cl_data->activePopulation();

    QVector< QVector<QString> > nglst;

    for (int i = 0; i < cl_data->numPopulations(); ++i)
    {
        QVector<QString> vec;
        cl_data->setActivePopulation(i);
        for (int i = 0; i < cl_data->numGenes(); ++i)
            vec.push_back(cl_data->nameGene(i));
        nglst.push_back(vec);
    }

    QVector<QString> strlst = geneNameList(nglst);
    QStringList nameGenes;
    for (int i = 0; i < strlst.size(); ++i)
    {
        nameGenes.push_back(strlst[i]);
        qDebug() << strlst[i];
    }

    GeneDialog *dialog = new GeneDialog;
    dialog->setMulitpleSelection(true);
    dialog->setGeneList(nameGenes);
    if (dialog->exec() == QDialog::Accepted)
    {
        qDebug() << pI->name();
        pI->tblResult(cl_data, dialog->getGene());
    }
    delete dialog;

    cl_data->setActivePopulation(curSelectedPop);
}

QVector<QString> Program::geneNameList(QVector<QVector<QString> > nglst)
{
    QVector<QString> minDict = nglst[0];

    for (int k = 1; k < nglst.size(); ++k)
    {
        int i = 0;
        while (i < minDict.size())
        {
            int f = 0;
            for (int j = 0; j < nglst[k].size(); ++j)
                if (minDict[i] == nglst[k].at(j))
                {
                    f = 1;
                    break;
                }
            if (f)
                i++;
            else
                minDict.remove(i);
        }
    }

    return minDict;
}
