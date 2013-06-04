#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "main.h"
#include "databox.h"


const int stretchCoef = 200;

class Graphics : public QSplitter
{
    Q_OBJECT
private:
    int sizeOfElem;
    QRectF cl_rect;
    QGraphicsScene *cl_sceneH;
    QGraphicsView *cl_viewH;
    QGraphicsScene *cl_sceneI;
    QGraphicsView *cl_viewI;
    void drawAxis(int width, int height);
public:
    Graphics(QWidget *pwgt = 0);
    void drawGraphFrequency(DataBox *data);
    void drawGraphRR(DataBox *data);
};

double maxHeight(DataBox *data, int curColumn);

#endif // GRAPHICS_H
