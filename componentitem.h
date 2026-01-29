#ifndef COMPONENTITEM_H
#define COMPONENTITEM_H

#include <QGraphicsPixmapItem>
#include <QPixmap>

class ComponentItem :public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit ComponentItem(const QPixmap& pixmap,
                           QGraphicsItem* parent = nullptr);

    virtual ~ComponentItem() = default;

    // Has to be implemented by all components
    virtual void updateState() = 0;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // COMPONENTITEM_H
