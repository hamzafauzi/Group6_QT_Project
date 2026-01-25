#include "componentitem.h"

// Basic-Class for every Component
// it is perfect for scalability

ComponentItem::ComponentItem(const QPixmap& pixmap,
                             QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlags(
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemSendsScenePositionChanges
        );
}
