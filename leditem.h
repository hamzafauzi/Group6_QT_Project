#ifndef LEDITEM_H
#define LEDITEM_H

#include "componentitem.h"
#include "pinitem.h"

class LedItem : public ComponentItem
{
    Q_OBJECT
public:
    explicit LedItem(QGraphicsItem* parent = nullptr);

    PinItem* signalPin() const { return m_signalPin; }
    PinItem* groundPin() const { return m_groundPin; }

    void updateState() override;

    // Simulation
    void setOn(bool on);

private:
    PinItem* m_signalPin;
    PinItem* m_groundPin;
    bool m_isOn;
};

#endif // LEDITEM_H
