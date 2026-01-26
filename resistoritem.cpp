#include "resistoritem.h"
#include <QPixmap>

ResistorItem::ResistorItem(QGraphicsItem* parent)
    : ComponentItem(QPixmap(":/resources/images/resistor.png"), parent),
    m_resistance(220)
{
    QPixmap pix = pixmap().scaledToWidth(100, Qt::SmoothTransformation);
    setPixmap(pix);
    
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Create pins
    m_leftPin = new PinItem(0, PinItem::Digital, this);
    m_leftPin->setPos(0, 35);

    m_rightPin = new PinItem(0, PinItem::Digital, this);
    m_rightPin->setPos(90, 35);
}

void ResistorItem::updateState()
{
    // Resistor doesn't change visual state
}
