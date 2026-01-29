#include "componentitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

// Basic-Class for every Component
// it is perfect for scalability

ComponentItem::ComponentItem(const QPixmap& pixmap,
                             QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(pixmap, parent)
{
    setFlags(
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemSendsScenePositionChanges
        );
}

void ComponentItem::mousePressEvent(QGraphicsSceneMouseEvent* event){
    if(event->button() == Qt::RightButton){
        // Deleting with scene
        if(scene()){
            scene()->removeItem(this);
        }

        delete this;
        return;
    }

    QGraphicsPixmapItem::mousePressEvent(event);
}
