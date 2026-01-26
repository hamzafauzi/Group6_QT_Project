#ifndef RESISTORITEM_H
#define RESISTORITEM_H

#include "componentitem.h"
#include "pinitem.h"

class ResistorItem : public ComponentItem
{
    Q_OBJECT
public:
    explicit ResistorItem(QGraphicsItem* parent = nullptr);

    PinItem* leftPin() const { return m_leftPin; }
    PinItem* rightPin() const { return m_rightPin; }
    void setResistance(int ohms) { m_resistance = ohms; }
    int resistance() const { return m_resistance; }

    void updateState() override;

private:
    PinItem* m_leftPin;
    PinItem* m_rightPin;
    int m_resistance;
};

#endif // RESISTORITEM_H
