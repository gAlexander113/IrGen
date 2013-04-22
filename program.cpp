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

void Program::slotOpen() // может отображение необработанных данных стоит убрать
{
    tblWgtInfo->clear();
    cl_data->loadData();
//    cl_data->output();

    QStringList lst;

    for (int i = 0; i < cl_data->numGenes(); ++i)
        lst << cl_data->nameGene(i);

    lst << QObject::tr("Количество здоровых") << QObject::tr("Количество больных");

    tblWgtInfo->setColumnCount(cl_data->numGenes() + 2);
    tblWgtInfo->setRowCount(cl_data->numAlleles());
    tblWgtInfo->setHorizontalHeaderLabels(lst);

    QTableWidgetItem *pitem = 0;

    qDebug() << tblWgtInfo->rowCount() << tblWgtInfo->columnCount();
    tblWgtInfo->resizeColumnsToContents();
    int row = tblWgtInfo->rowCount();
    int col = tblWgtInfo->columnCount();
    for (int i = 0; i < row; ++i)
    {
//        tblWgtInfo->insertRow(i);
//        qDebug() << "row " << i;
        for (int j = 0; j < col; ++j)
        {
//            qDebug() << "column " << j;
            if (j < cl_data->numGenes())
            {
                QString str = cl_data->allele(j, i);
                if (str == "0,0")
                    str = "";
                pitem = new QTableWidgetItem(str);
            }
            else
                if (j == cl_data->numGenes())
                    pitem = new QTableWidgetItem(cl_data->numH(0, i));
            else
                    if (j == cl_data->numGenes() + 1)
                        pitem = new QTableWidgetItem(cl_data->numI(0, i));

            pitem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            tblWgtInfo->setItem(i, j, pitem);
        }
        tblWgtInfo->resizeRowsToContents();
    }
    cl_status->setText(tr("Данные загруженны."));
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
        if (cl_data->data(index, Qt::DisplayRole).toString().size() != 2) // если не пара аллелей :)
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


