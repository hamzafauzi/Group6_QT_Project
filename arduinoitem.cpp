#include "arduinoitem.h"
#include <QPixmap>
#include <QGraphicsItem>
#include "pinitem.h"

ArduinoItem::ArduinoItem(QGraphicsItem* parent)
    : ComponentItem(QPixmap(":/resources/images/arduino_board.png"), parent)
{
    QPixmap pix = pixmap().scaled(600, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(pix);
    setPos(100, 100);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    createPins();
}

void ArduinoItem::createPins(){
    // LED-Pin
    PinItem* inbuildLedPin = new PinItem(13, PinItem::Led, this);
    inbuildLedPin->setPos(220, 75);
    // Pins-Top
    PinItem* Pin2 = new PinItem(2, PinItem::Digital, this);
    Pin2->setPos(420, 10);
    m_Pins.insert(0, Pin2);
    PinItem* Pin3 = new PinItem(3, PinItem::Analog, this);
    Pin3->setPos(405, 10);
    m_Pins.insert(1, Pin3);
    PinItem* Pin4 = new PinItem(4, PinItem::Digital, this);
    Pin4->setPos(385, 10);
    m_Pins.insert(2, Pin4);
    PinItem* Pin5 = new PinItem(5, PinItem::Analog, this);
    Pin5->setPos(370, 10);
    m_Pins.insert(3, Pin5);
    PinItem* Pin6 = new PinItem(6, PinItem::Analog, this);
    Pin6->setPos(355, 10);
    m_Pins.insert(4, Pin6);
    PinItem* Pin7 = new PinItem(7, PinItem::Digital, this);
    Pin7->setPos(340, 10);
    m_Pins.insert(5, Pin7);
    PinItem* Pin8 = new PinItem(8, PinItem::Digital, this);
    Pin8->setPos(310, 10);
    m_Pins.insert(6, Pin8);
    PinItem* Pin9 = new PinItem(9, PinItem::Analog, this);
    Pin9->setPos(295, 10);
    m_Pins.insert(7, Pin9);
    PinItem* Pin10 = new PinItem(10, PinItem::Analog, this);
    Pin10->setPos(275, 10);
    m_Pins.insert(8, Pin10);
    PinItem* Pin11 = new PinItem(11, PinItem::Analog, this);
    Pin11->setPos(260, 10);
    m_Pins.insert(9, Pin11);
    PinItem* Pin12 = new PinItem(12, PinItem::Digital, this);
    Pin12->setPos(245, 10);
    m_Pins.insert(10, Pin12);
    PinItem* Pin13 = new PinItem(13, PinItem::Digital, this);
    Pin13->setPos(225, 10);
    m_Pins.insert(11, Pin13);
    // Ground
    m_groundPin = new PinItem(PinItem::Ground, this);
    m_groundPin->setPos(210, 10);
}

void ArduinoItem::updateState()
{
    // später: Arduino-spezifische Logik
}

PinItem* ArduinoItem::getPin(int number){
    for (PinItem* pin : m_Pins) {
        if (pin->pinNumber() == number)
            return pin; // found
    }
    return nullptr; // not found
}

PinItem* ArduinoItem::groundPin(){
    return m_groundPin;
}
