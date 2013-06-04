#include "databox.h"

DataBox::DataBox(int nrow, int ncol, QObject *pobj)
    : QAbstractTableModel(pobj)
    , cl_nrow(nrow)
    , cl_ncolumn(ncol)
    , cl_activePop(0)
{
    im = new QImage(500, 500, QImage::Format_ARGB32);
}

void DataBox::traverseNode(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    while (!domNode.isNull())
    {
        if (domNode.isElement())                        
        {
            QDomElement domElement = domNode.toElement();

            if (domElement.tagName() == "population")
            {
                Population popdata;
                popdata.name = domElement.attribute("name");
                cl_genes.push_back(popdata);
//                popInd++;
//                geneInd = 0;
            }

            if (domElement.tagName() == "gene")
            {
                Gene gene;
                gene.name = domElement.attribute("name");
                cl_genes[cl_genes.size() - 1].genes.push_back(gene);
//                geneInd++;
            }

            if (domElement.tagName() == "allele")
            {
                AlleleElement tmpAl;
                tmpAl.allele = domElement.attribute("name");
                tmpAl.numIll = domElement.attribute("Ill");
                tmpAl.numHealthy = domElement.attribute("Healthy");
                cl_genes[cl_genes.size() - 1].genes[cl_genes[cl_genes.size() - 1].genes.size() - 1].alleles.push_back(tmpAl);
            }
        }
        traverseNode(domNode);
        domNode = domNode.nextSibling();
    }
}


void DataBox::loadData(QString filePath)
{
    cl_genes.clear();
    QDomDocument domDoc;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            traverseNode(domElement);
        }
        file.close();
    }
    else
        qDebug() << "file is not open";
}

void DataBox::loadData(QSqlQuery *base)
{
}

void DataBox::output()
{
    for (int i = 0; i < cl_genes.size(); ++i)
    {        
        qDebug() << cl_genes[i].name;
        for (int k = 0; k < cl_genes[i].genes.size(); ++k)
        {
            qDebug() << cl_genes[i].genes[k].name;
            for (int j = 0; j < cl_genes[i].genes[k].alleles.size(); ++j)
                qDebug() << cl_genes[i].genes[k].alleles[j].allele << cl_genes[i].genes[k].alleles[j].numHealthy
                         << cl_genes[i].genes[k].alleles[j].numIll;
        }
    }
}

void DataBox::saveData() // TODO :: потом поменять сохранялку на более человеческий вид
{    
    QString filename = QFileDialog::getSaveFileName(0, tr("Сохранить"), "", "*.html");
    QString str;

    str = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Strict//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
           "<html>\n"
           "<head>\n"
            " <style type=\"text/css\">"
            "table { border-spacing: 0; padding:0; border: 1px solid black; }"
            "th { background-color: #dddddd; padding: 7px; }"
            "td { border-top: 1px solid #dddddd; padding: 7px; }"
            "span.red { color: red; font-weight: bold; }"
            "span.green { color: green; font-weight: bold; }"
            "</style>"
            "</head>\n"
            "<body>\n"
            "<h2>Результаты</h2>\n"
            "<p> Популяция: " + cl_genes[cl_activePop].name + "<br>\n"
            "Ген: " + cl_data[0].geneName + "</p>"
            "<table>\n"
            "<tr>\n"
            "<th>Наименование</th>\n"
            "<th>Группа</th>\n"
            "<th>Здоровые кол-во</th>\n"
            "<th>Больные кол-во</th>\n"
            "<th>Здоровые частота</th>\n"
            "<th>Больные частота</th>\n"
            "<th>Отношение шансов</th>\n"
            "<th>95% доверит. инт.</th>\n"
            "</tr>\n");

    for (int i = 0; i < cl_data.size(); ++i)
    {
        str += QString("<tr>\n");
        str += QString("<td>") + cl_data[i].name + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].group) + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].numHealthy) + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].numIll) + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].freakHealthy) + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].freakIll) + QString("</td>");
        str += QString("<td>") + QString::number(cl_data[i].RR) + QString("</td>");
        str += QString("<td>");
        int length = cl_data[i].RRInt.size();
        QString s = cl_data[i].RRInt.mid(1, length - 2);
        QStringList strlst = s.split(";");
        double left = strlst[0].toDouble();
        double right = strlst[1].toDouble();
        if (cl_data[i].RR > 1 && left > 1)
            str += QString("<span class=\"red\">");
        else
            if (cl_data[i].RR < 1 && right < 1)
                str += QString("<span class=\"green\">");
        else
                str += QString("<span>");
        str += cl_data[i].RRInt;
        str += QString("</span></td>");

        str += QString("</tr>\n");        
    }

    str  += QString("</table>\n"
            "</body>\n"
            "</html>\n");

    if (!filename.contains(".html"))
        filename += ".html";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream streamer(&file);
        streamer << str;
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
        case 8: // имя гена
            return cl_data[index.row()].geneName;
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
        case 8:
            cl_data[index.row()].geneName = value.toString();
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
        case 6: str = QObject::tr("Отношение шансов(OR)");
                break;
        case 7: str = QObject::tr("95% доверит. инт.(OR)");
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


