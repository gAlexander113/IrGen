#include "graphicstextitem.h"

GraphicsTextItem::GraphicsTextItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent)
{
}

GraphicsTextItem::GraphicsTextItem(QString text, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent)
{
}

void GraphicsTextItem::setFixedSize(qreal x, qreal y, qreal width, qreal height)
{
    fixedRect.setRect(x, y, width, height);
}

QRectF GraphicsTextItem::boundingRect() const
{
    return fixedRect;
}
