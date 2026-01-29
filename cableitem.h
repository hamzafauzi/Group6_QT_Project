#ifndef CABLEITEM_H
#define CABLEITEM_H

#include <QGraphicsLineItem>
#include "pinitem.h"

class CableItem : public QGraphicsLineItem
{
public:
    CableItem(PinItem* from, PinItem* to, QGraphicsItem* parent = nullptr);
    ~CableItem();
    void updateLine();
    void propagateSignal(PinItem* fromPin);
    void setHigh();
    void setLow();

private:
    PinItem* m_from;
    PinItem* m_to;
    bool m_isHigh = false;
};

#endif // CABLEITEM_H
