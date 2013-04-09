#include "graphics.h"

Graphics::Graphics(QWidget *pwgt)
    : QWidget(pwgt)
{
//    cl_rect.setSize(QSizeF(500, 500));
    cl_rect.setRect(0, 0, 500, 500);
    cl_scene = new QGraphicsScene(cl_rect);
    cl_view = new QGraphicsView(cl_scene, this);

    this->resize(cl_rect.width() ,cl_rect.height());
}

void Graphics::drawGraphFrequency(DataBox *data)
{
}

void Graphics::drawGraphRR(DataBox *data)
{
}


void Graphics::drawAxis()
{
    /************************/
    /*      draw OX OY      */
    /************************/
    cl_scene->addLine(10, cl_rect.y() - 10, cl_rect.x() - 10, cl_rect.y() - 10, QPen(Qt::black, 1)); // OX
    cl_scene->addLine(10, cl_rect.y() - 10, 10, 10, QPen(Qt::black, 1)); // OY
}
