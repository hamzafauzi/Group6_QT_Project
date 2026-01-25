#include "leditem.h"

LedItem::LedItem(QGraphicsItem* parent)
    : ComponentItem(QPixmap(":/resources/images/led_off.png"), parent),
    m_isOn(false)
{
    // LED scale
    setPixmap(pixmap().scaledToWidth(80, Qt::SmoothTransformation));

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // create Pins
    // Signal (Anode)
    m_signalPin = new PinItem(0, PinItem::PinType::Digital, this);
    m_signalPin->setPos(20, 70);

    // Ground (Kathode)
    m_groundPin = new PinItem(PinItem::PinType::Ground, this);
    m_groundPin->setPos(50, 70);

    connect(m_signalPin, &PinItem::stateChanged, this, &LedItem::setOn);
}

void LedItem::setOn(bool on)
{
    m_isOn = on;
    updateState();
}

void LedItem::updateState()
{
    if (m_isOn)
        setPixmap(QPixmap(":/resources/images/led_on.png")
                      .scaledToWidth(80, Qt::SmoothTransformation));
    else
        setPixmap(QPixmap(":/resources/images/led_off.png")
                      .scaledToWidth(80, Qt::SmoothTransformation));
}

