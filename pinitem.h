#ifndef PINITEM_H
#define PINITEM_H

#pragma once
#include "componentitem.h"
#include <QBrush>

class CableItem;

class PinItem : public ComponentItem
{
    Q_OBJECT
public:
    enum PinType { Digital, Analog, Led, Ground, Power };

    // for digital / analog
    PinItem(int pinNumber, PinType type = Digital, QGraphicsItem* parent = nullptr);

    // for GND / Power
    PinItem(PinType, QGraphicsItem* parent = nullptr);

    void updateState() override;   // Pflicht für ComponentItem

    // Signals
    void setHigh();
    void setLow();
    bool isHigh() const {return m_isHigh;}

    // Cable management
    void addCable(CableItem* cable);

    // Getter
    int pinNumber() const { return m_pinNumber; }
    PinType pinType() const {return m_type;}

signals:
    void stateChanged(bool high); // High / Low Signal

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int m_pinNumber;
    PinType m_type;
    bool m_isHigh = false;

    QSet<CableItem*> m_cables;
};


#endif // PINITEM_H
