#ifndef ARDUINOITEM_H
#define ARDUINOITEM_H

#include "componentitem.h"
#include "pinitem.h"

class ArduinoItem : public ComponentItem
{
public:
    explicit ArduinoItem(QGraphicsItem* parent = nullptr);

    PinItem* getPin(int number);
    PinItem* groundPin();

    void updateState() override;

private:
    void createPins();

    QMap<int, PinItem*> m_Pins;
    PinItem* m_groundPin = nullptr;
};

#endif // ARDUINOITEM_H
