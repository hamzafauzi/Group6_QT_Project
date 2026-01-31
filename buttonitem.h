#ifndef BUTTONITEM_H
#define BUTTONITEM_H

#include "componentitem.h"
#include "pinitem.h"

class ButtonItem : public ComponentItem
{
    Q_OBJECT
public:
    explicit ButtonItem(QGraphicsItem* parent = nullptr);

    PinItem* pin_top_left() const { return m_pin_top_left; }
    PinItem* pin_top_right() const { return m_pin_top_right; }
    PinItem* pin_bottom_left() const { return m_pin_bottom_left; }
    PinItem* pin_bottom_right() const { return m_pin_bottom_right; }

    void onPinChanged(PinItem* changedPin);

    void updateState() override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    PinItem* m_pin_top_left;
    PinItem* m_pin_top_right;
    PinItem* m_pin_bottom_left;
    PinItem* m_pin_bottom_right;

    bool m_pressed = false;
    bool m_internal = false; // protection from recursion
    void reevaluateAfterRelease();
    void syncUpper();
    void syncLower();
    void syncAll();
};

#endif // BUTTONITEM_H
