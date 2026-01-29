#include "pinitem.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "cableitem.h"
#include "componentitem.h"
#include "leditem.h"
#include "resistoritem.h"
#include "buttonitem.h"
#include "arduinoitem.h"
#include "pininfodialog.h"
#include "diagram.h"

PinItem::PinItem(int pinNumber, PinType type, QGraphicsItem* parent)
    : ComponentItem(QPixmap(), parent), m_pinNumber(pinNumber), m_type(type)
{
    // shown as small circle
    setPixmap(QPixmap(10, 10));
    QPixmap pix(10, 10);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    // Color after Pin-Type
    switch (m_type) {
    case PinType::Ground:
        painter.setBrush(Qt::darkGray);
        break;
    case PinType::Power:
        painter.setBrush(Qt::red);
        break;
    case PinType::Led:
        painter.setBrush(Qt::white);
        break;
    default:
        painter.setBrush(Qt::gray);
        break;
    }
    painter.setPen(QPen(Qt::black));
    painter.drawEllipse(0,0,10,10);
    painter.end();

    setPixmap(pix);

    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true); // Allow selection so clicks work
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // Make sure pins can receive mouse events
    setAcceptHoverEvents(true);
}

PinItem::PinItem(PinType type, QGraphicsItem* parent)
    : ComponentItem(QPixmap(), parent), m_pinNumber(-1), m_type(type)
{
    // shown as small circle
    setPixmap(QPixmap(10, 10));
    QPixmap pix(10, 10);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    // Color after Pin-Type
    switch (m_type) {
    case PinType::Ground:
        painter.setBrush(Qt::darkGray);
        break;
    case PinType::Power:
        painter.setBrush(Qt::red);
        break;
    case PinType::Led:
        painter.setBrush(Qt::white);
        break;
    default:
        painter.setBrush(Qt::gray);
        break;
    }
    painter.setPen(QPen(Qt::black));
    painter.drawEllipse(0,0,10,10);
    painter.end();

    setPixmap(pix);

    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true); // Allow selection so clicks work
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // Make sure pins can receive mouse events
    setAcceptHoverEvents(true);
}

void PinItem::updateState()
{
    // here can be logic, e.g. change color
}

void PinItem::setHigh()
{
    if (m_isHigh) return; // allready HIGH → do nothing
    m_isHigh = true;

    QPixmap pixmap(10,10);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setBrush(Qt::green); // HIGH = grün
    p.drawEllipse(0,0,10,10);
    p.end();
    setPixmap(pixmap);

    emit stateChanged(true);

    // inform all cables
    for(CableItem* cable : m_cables){
        cable->setHigh();
    }
}

void PinItem::setLow()
{
    if (!m_isHigh) return; // allready LOW → do nothing
    m_isHigh = false;

    QPixmap pixmap(10,10);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setBrush(Qt::red); // LOW = rot
    p.drawEllipse(0,0,10,10);
    p.end();
    setPixmap(pixmap);

    emit stateChanged(false);

    // inform all cables
    for(CableItem* cable : m_cables){
        cable->setLow();
    }
}

void PinItem::addCable(CableItem* cable){
    m_cables.insert(cable);
}

void PinItem::removeCable(CableItem* cable){
    m_cables.remove(cable);
}

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant& value){
    if (change == ItemScenePositionHasChanged)
    {
        for (CableItem* cable : m_cables)
            cable->updateLine();
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

PinItem::~PinItem(){
    for (CableItem* cable : m_cables){
        if(scene())
            scene()->removeItem(cable);
        delete cable;
    }
    m_cables.clear();
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // z.B. nur auswählen, NICHT löschen
    event->accept();
}
