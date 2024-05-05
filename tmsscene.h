#ifndef TMSSCENE_H
#define TMSSCENE_H

#include <QGraphicsScene>

class TMSScene : public QGraphicsScene
{
public:
    explicit TMSScene(QObject *parent = nullptr);

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    qreal scaleFactor = 1.03;
    bool controlPressed = false;
};

#endif // TMSSCENE_H
