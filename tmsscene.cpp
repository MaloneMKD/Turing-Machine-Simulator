#include "tmsscene.h"
#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QGraphicsView>

TMSScene::TMSScene(QObject *parent)
    : QGraphicsScene(parent)
{}

void TMSScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QList<QGraphicsView*> views = this->views();
    if(event->delta() > 0){ // Scroll up
        if(controlPressed)
        {
            views[0]->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            views[0]->scale(scaleFactor, scaleFactor);
            event->accept();
        }
    }
    else // Scroll down
    {
        if(controlPressed)
        {
            views[0]->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            views[0]->scale(1/scaleFactor, 1/scaleFactor);
            event->accept();
        }
    }
}

void TMSScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
        controlPressed = true;
}

void TMSScene::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
        controlPressed = false;
}

