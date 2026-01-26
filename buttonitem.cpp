#include "buttonitem.h"
#include <QPixmap>

ButtonItem::ButtonItem(QGraphicsItem* parent)
    : ComponentItem(QPixmap(":/resources/images/button_up.png"), parent)
{
    QPixmap pix = pixmap().scaledToWidth(80, Qt::SmoothTransformation);
    setPixmap(pix);
    
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Create 4 pins
    m_pin1 = new PinItem(0, PinItem::Digital, this);  // Top-left
    m_pin1->setPos(0, 20);

    m_pin2 = new PinItem(0, PinItem::Digital, this);  // Bottom-left
    m_pin2->setPos(0, 40);

    m_pin3 = new PinItem(0, PinItem::Digital, this);  // Top-right
    m_pin3->setPos(70, 20);

    m_pin4 = new PinItem(0, PinItem::Digital, this);  // Bottom-right
    m_pin4->setPos(70, 40);
}

void ButtonItem::updateState()
{
    // Button visual state can be updated here if needed
}
