#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>

class GraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
private:
    QRectF fixedRect;
public:
    GraphicsTextItem(QGraphicsItem *parent = 0);
    GraphicsTextItem(QString text, QGraphicsItem *parent = 0);
    void setFixedSize(qreal x, qreal y, qreal width, qreal height);
    QRectF boundingRect() const;
};

#endif // GRAPHICSTEXTITEM_H
