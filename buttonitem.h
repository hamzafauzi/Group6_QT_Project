#ifndef BUTTONITEM_H
#define BUTTONITEM_H

#include "componentitem.h"
#include "pinitem.h"

class ButtonItem : public ComponentItem
{
    Q_OBJECT
public:
    explicit ButtonItem(QGraphicsItem* parent = nullptr);

    PinItem* pin1() const { return m_pin1; }
    PinItem* pin2() const { return m_pin2; }
    PinItem* pin3() const { return m_pin3; }
    PinItem* pin4() const { return m_pin4; }

    void updateState() override;

private:
    PinItem* m_pin1;
    PinItem* m_pin2;
    PinItem* m_pin3;
    PinItem* m_pin4;
};

#endif // BUTTONITEM_H
