#include "databox.h"

DataBox::DataBox(int nrow, int ncol, QObject *pobj)
    : QAbstractTableModel(pobj)
    , cl_nrow(nrow)
    , cl_ncolumn(ncol)
{
}

void DataBox::traverseNode(const QDomNode &node, QString ill, QString healthy)
{
    QDomNode domNode = node.firstChild();

    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (domElement.tagName() == "set")
            {
                healthy = domElement.attribute("healthy", "");
                ill = domElement.attribute("ill", "");
            }
            else
                if (domElement.tagName() == "gene")
                {
                    int found = 0;

                    int i;
                    for (i = 0; i < cl_genes.size(); ++i)
                    {
                        if (cl_genes[i].name == domElement.attribute("name"))
                        {
                            found = 1;
                            break;
                        }
                    }

                    if (!found)
                    {
                        Gene gene;
                        gene.name = domElement.attribute("name");

                        AlleleElement al;
                        al.allele = domElement.attribute("alleles");
                        al.numHealthy = healthy;
                        al.numIll = ill;

                        gene.alleles.push_back(al);

                        cl_genes.push_back(gene);
                    }
                    else
                    {
                        AlleleElement al;
                        al.allele = domElement.attribute("alleles");
                        al.numHealthy = healthy;
                        al.numIll = ill;

                        cl_genes[i].alleles.push_back(al);
                    }
                }
        }
        traverseNode(domNode, ill, healthy);
        domNode = domNode.nextSibling();
    }
}

void DataBox::loadData()
{
    QString pathFile = QFileDialog::getOpenFileName(0, QObject::tr("Открыть файл"), "", "*.xml");

    QDomDocument domDoc;
    QFile file(pathFile);
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            traverseNode(domElement, 0, 0);
        }
        file.close();
    }
    else
        qDebug() << "file is not open";
}

void DataBox::output()
{
    for (int i = 0; i < cl_genes.size(); ++i)
    {
        qDebug() << cl_genes[i].name;
        for (int j = 0; j < cl_genes[i].alleles.size(); ++j)
            qDebug() << cl_genes[i].alleles[j].allele << cl_genes[i].alleles[j].numHealthy
                     << cl_genes[i].alleles[j].numIll;
    }
}

void DataBox::saveData()
{
    QDomDocument doc("data");
    QDomElement domElemdata = doc.createElement("data");

    doc.appendChild(domElemdata);

    for (int i = 0; i < numAlleles(); ++i)
    {
        QDomElement domElSet = doc.createElement("set");

        QDomAttr domAttrI = doc.createAttribute("ill");
        domAttrI.setValue(numI(0, i));

        QDomAttr domAttrH = doc.createAttribute("healthy");
        domAttrH.setValue(numH(0, i));

        domElSet.setAttributeNode(domAttrI);
        domElSet.setAttributeNode(domAttrH);

        for (int j = 0; j < numGenes(); ++j)
        {
            QDomElement domElgene = doc.createElement("gene");

            QDomAttr domAttrAl = doc.createAttribute("alleles");
            domAttrAl.setValue(allele(j, i));

            QDomAttr domAttrName = doc.createAttribute("name");
            domAttrName.setValue(nameGene(j));

            domElgene.setAttributeNode(domAttrAl);
            domElgene.setAttributeNode(domAttrName);

            domElSet.appendChild(domElgene);
        }

        domElemdata.appendChild(domElSet);
    }


    QString str = QFileDialog::getSaveFileName(0, QObject::tr("Сохранить"), "", "*.xml");

    if (str.isEmpty())
    {
        qDebug() << "no file for save";
        return;
    }

    if (!str.contains("xml"))
        str += ".xml";

    QFile file(str);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}


QVariant DataBox::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0: // наименование
            return cl_data[index.row()].name;
        case 1: // группа
            return cl_data[index.row()].group;
        case 2: // здоровые кол-во
            return cl_data[index.row()].numHealthy;
        case 3: // больные кол-во
            return cl_data[index.row()].numIll;
        case 4: // здоровые частота
            return cl_data[index.row()].freakHealthy;
        case 5: // больные частота
            return cl_data[index.row()].freakIll;
        case 6: // относ риск
            return cl_data[index.row()].RR;
        case 7: // 95% интервал
            return cl_data[index.row()].RRInt;
        default:
            qDebug() << "no columns to show";
            break;
        }
    }
    if (role == Qt::BackgroundRole)
    {
        if (index.column() == 7)
        {
            int length = cl_data[index.row()].RRInt.size();
            QString str = cl_data[index.row()].RRInt.mid(1, length - 2);
            QStringList strlst = str.split(";");
            double left = strlst[0].toDouble();
            double right = strlst[1].toDouble();
            if (cl_data[index.row()].RR > 1 && left > 1)
                return (QVariant(QColor(Qt::red)));
            else
                if (cl_data[index.row()].RR < 1 && right < 1)
                    return (QVariant(QColor(Qt::green)));

            return QVariant();
        }
    }
    return QVariant();
}

bool DataBox::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0: // наименование
            cl_data[index.row()].name = value.toString();
            break;
        case 1: // группа
            cl_data[index.row()].group = value.toInt();
            break;
        case 2: // здоровые кол-во
            cl_data[index.row()].numHealthy = value.toInt();
            break;
        case 3: // больные кол-во
            cl_data[index.row()].numIll = value.toInt();
            break;
        case 4: // здоровые частота
            cl_data[index.row()].freakHealthy = value.toDouble();
            break;
        case 5: // больные частота
            cl_data[index.row()].freakIll = value.toDouble();
            break;
        case 6: // относ риск
            cl_data[index.row()].RR = value.toDouble();
            break;
        case 7: // 95% интервал
            cl_data[index.row()].RRInt = value.toString();
            break;
        default:
            qDebug() << "no columns to show";
            break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

int DataBox::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return cl_nrow;
}

int DataBox::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return cl_ncolumn;
}

QVariant DataBox::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    QString str;
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: str = QObject::tr("Наименование");
                break;
        case 1: str = QObject::tr("Группа");
                break;
        case 2: str = QObject::tr("Здоровые кол-во");
                break;
        case 3: str = QObject::tr("Больные кол-во");
                break;
        case 4: str = QObject::tr("Здоровые частота");
                break;
        case 5: str = QObject::tr("Больные частота");
                break;
        case 6: str = QObject::tr("Отношение шансов(RR)");
                break;
        case 7: str = QObject::tr("95% доверит. инт.(RR)");
                break;
        default :
            qDebug() << "ERROR: No such column";
            break;
        }
    }
    else
        str = QString::number(section + 1);

    return str;
}


Qt::ItemFlags DataBox::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsSelectable)
                           : flags;
}


