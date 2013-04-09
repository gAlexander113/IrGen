#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "main.h"
#include "databox.h"

class Graphics : public QWidget
{
    Q_OBJECT
private:
    QRectF cl_rect;
    QGraphicsScene *cl_scene;
    QGraphicsView *cl_view;
    void drawAxis();
public:
    Graphics(QWidget *pwgt = 0);
    void drawGraphFrequency(DataBox *data);
    void drawGraphRR(DataBox *data);
};

#endif // GRAPHICS_H
