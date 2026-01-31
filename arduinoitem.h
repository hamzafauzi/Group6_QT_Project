#ifndef ARDUINOITEM_H
#define ARDUINOITEM_H

#include "componentitem.h"
#include "pinitem.h"

class ArduinoItem : public ComponentItem
{
    Q_OBJECT
public:
    explicit ArduinoItem(QGraphicsItem* parent = nullptr);

    PinItem* getPin(int number);
    PinItem* groundPin();

    void updateState() override;

private:
    void createPins();

    QMap<int, PinItem*> m_Pins;
    PinItem* m_groundPin = nullptr;
    PinItem* m_powerPin = nullptr;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // ARDUINOITEM_H
