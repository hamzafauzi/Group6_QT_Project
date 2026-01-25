#include "simulatorscene.h"

#include <QPainter>
#include <QPen>

SimulatorScene::SimulatorScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 1200, 800);

    setBackgroundBrush(QColor(245, 245, 245));
}

void SimulatorScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    // grid for placing
    const int gridSize = 20;

    QPen pen(QColor(220, 220, 220));
    pen.setWidth(1);
    painter->setPen(pen);

    const QRectF scene = sceneRect();

    for (int x = int(scene.left()); x < scene.right(); x += gridSize)
        painter->drawLine(x, scene.top(), x, scene.bottom());

    for (int y = int(scene.top()); y < scene.bottom(); y += gridSize)
        painter->drawLine(scene.left(), y, scene.right(), y);
}

