#ifndef ROUNDEDBUTTON_H
#define ROUNDEDBUTTON_H

#include <QObject>
#include <QGraphicsTextItem>
#include "qgraphicsroundedrectitem.h"

class RoundedButton :public QGraphicsRoundedRectItem
{
    Q_OBJECT
public:
    explicit RoundedButton(QObject *parent = nullptr, QGraphicsItem *parentItem = nullptr, QRectF rect = QRectF(), QString label = "");

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QGraphicsTextItem *m_Label;
};

#endif // ROUNDEDBUTTON_H
