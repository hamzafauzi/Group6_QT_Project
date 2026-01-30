#include "cableitem.h"
#include "pinitem.h"
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

CableItem::CableItem(PinItem* from, PinItem* to, QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
    m_from(from),
    m_to(to)
{
    setPen(QPen(Qt::black, 4));
    m_from->addCable(this);
    m_to->addCable(this);
    updateLine();
}

void CableItem::updateLine()
{
    if (!m_from || !m_to)
        return;

    // Scene-Positionen der Pins
    QPointF p1 = m_from->scenePos();
    QPointF p2 = m_to->scenePos();

    // Gerade Linie setzen
    QLineF line(p1, p2);
    setLine(line);
}

void CableItem::setHigh()
{
    m_isHigh = true;
    setPen(QPen(Qt::green, 4)); // dicke grüne Linie für HIGH
    // LED am anderen Ende informieren
    m_to->setHigh();
}

void CableItem::setLow()
{
    m_isHigh = false;
    setPen(QPen(Qt::black, 4)); // normale Linie
    m_to->setLow();
}

void CableItem::propagateSignal(PinItem* fromPin)
{
    PinItem* otherPin = (fromPin == m_from) ? m_to : m_from;
    if (!otherPin) return;

    // GND ovverrides everything
    if (m_from->type() == PinItem::PinType::Ground ||
        m_to->type() == PinItem::PinType::Ground)
    {
        m_from->setLow();
        m_to->setLow();
        return;
    }

    // HIGH weitergeben
    if (fromPin->isHigh())
        otherPin->setHigh();
    else
        otherPin->setLow();
}

CableItem::~CableItem(){
    if(m_from) m_from->removeCable(this);
    if(m_to) m_to->removeCable(this);
}

void CableItem::mousePressEvent(QGraphicsSceneMouseEvent* event){
    if(event->button() == Qt::RightButton){
        // remove cable from pins
        if(m_from) m_from->removeCable(this);
        if(m_to) m_to->removeCable(this);

        scene()->removeItem(this);
        delete this;
        return;
    }
    QGraphicsLineItem::mousePressEvent(event);
}
