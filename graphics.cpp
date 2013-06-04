#include "graphics.h"

Graphics::Graphics(QWidget *pwgt)
    : QSplitter(pwgt)
{
//    cl_rect.setSize(QSizeF(500, 500));    
    sizeOfElem = 20;
    cl_sceneH = new QGraphicsScene;
    cl_viewH = new QGraphicsView(cl_sceneH);
    cl_sceneI = new QGraphicsScene;
    cl_viewI = new QGraphicsView(cl_sceneI);

    QWidget *pwgt1 = new QWidget;
    QLabel *lbl1 = new QLabel(tr("Здоровые"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(lbl1);
    layout->addWidget(cl_viewH);
    pwgt1->setLayout(layout);

    QWidget *pwgt2 = new QWidget;
    QLabel *lbl2 = new QLabel(tr("Больные"));
    layout = new QVBoxLayout;
    layout->addWidget(lbl2);
    layout->addWidget(cl_viewI);
    pwgt2->setLayout(layout);

    this->setOrientation(Qt::Vertical);
    this->addWidget(pwgt1);
    this->addWidget(pwgt2);

    this->resize(cl_rect.width() ,cl_rect.height());
}

void Graphics::drawGraphFrequency(DataBox *data)
{
    cl_sceneH->clear();
    cl_sceneI->clear();

    QModelIndex index = data->index(0, 0);
    QString str = data->data(index, Qt::DisplayRole).toString();
    int nn = str.count(',');
    qDebug() << nn;
    switch (nn)
    {
    case 0: sizeOfElem = 20;
            break;
    case 1: sizeOfElem = 40;
            break;
    case 2: sizeOfElem = 80;
            break;
    default:
        break;
    }

    index = data->index(0, 8);
    this->setWindowTitle(tr("Частота встречаемости аллеля"));

    int graphWidth = data->getRowCount() * sizeOfElem;
    int graphHeight = qRound(2 * maxHeight(data, 4) * stretchCoef);
    qDebug() << "height table = " << graphHeight;
    drawAxis(graphWidth + 2 * sizeOfElem, graphHeight/* + stretchCoef*/);

    for (int i = 0; i < data->getRowCount(); ++i)
    {
        index = data->index(i, 4);
        double height = data->data(index, Qt::DisplayRole).toDouble();
        QColor color(Qt::red);
        qDebug() << i << " param = " << height * 100;
        if (height * 100 > 10)
            color = color.lighter(100 + qRound(height * 100));
        else
            color = color.darker(100 + qRound(height * 1000));

        QRectF rect;
        rect.setRect((i + 1) * sizeOfElem, cl_rect.height() - height * stretchCoef, sizeOfElem, height * stretchCoef);
        cl_sceneH->addRect(rect, QPen(), QBrush(color));
        index = data->index(i, 0);
        QGraphicsTextItem *pitem = cl_sceneH->addText(data->data(index, Qt::DisplayRole).toString());
        pitem->setPos((i + 1) * sizeOfElem, cl_rect.height() + 3);
    }

    for (int i = 0; i < data->getRowCount(); ++i)
    {
        index = data->index(i, 5);
        double height = data->data(index, Qt::DisplayRole).toDouble();
        QColor color(Qt::red);
        qDebug() << i << " param = " << height * 100;
        if (height * 100 > 10)
            color = color.lighter(100 + qRound(height * 100));
        else
            color = color.darker(100 + qRound(height * 1000));

        QRectF rect;
        rect.setRect((i + 1) * sizeOfElem, cl_rect.height() - height * stretchCoef, sizeOfElem, height * stretchCoef);
        cl_sceneI->addRect(rect, QPen(), QBrush(color));
        index = data->index(i, 0);
        QGraphicsTextItem *pitem = cl_sceneI->addText(data->data(index, Qt::DisplayRole).toString());
        pitem->setPos((i + 1) * sizeOfElem, cl_rect.height() + 3);
    }


}

void Graphics::drawGraphRR(DataBox *data)
{
}


void Graphics::drawAxis(int width, int height)
{    
    /************************/
    /*      draw OX OY      */
    /************************/
    cl_rect.setRect(0, 0, width, height);
    cl_sceneH->setSceneRect(cl_rect);
    cl_viewH->setSceneRect(0, 0, width, height + 10);
    qDebug() << "size of image = " << cl_rect;

    QPen pen(Qt::black, 1);
    cl_sceneH->addLine(0, 0, 0, cl_rect.height(), pen);
    cl_sceneH->addLine(0, cl_rect.height(), cl_rect.width(), cl_rect.height(), pen);

    cl_sceneI->addLine(0, 0, 0, cl_rect.height(), pen);
    cl_sceneI->addLine(0, cl_rect.height(), cl_rect.width(), cl_rect.height(), pen);
}

double maxHeight(DataBox *data, int curColumn)
{
    QModelIndex index;
    double max = -1; // не боись частота < нуля не будет
    for (int i = 0; i < data->getRowCount(); ++i)
    {
        index = data->index(i, curColumn);
        if (data->data(index, Qt::DisplayRole).toDouble() > max)
            max = data->data(index, Qt::DisplayRole).toDouble();
    }
    qDebug() << "max = " << max;
    return max;
}
