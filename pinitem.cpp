#include "pinitem.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "cableitem.h"
#include "componentitem.h"
#include "simulatorscene.h"
#include "resistoritem.h"
#include "leditem.h"
#include "buttonitem.h"
#include <QTimer>

PinItem::PinItem(int pinNumber, PinType type, QGraphicsItem* parent)
    : ComponentItem(QPixmap(), parent), m_pinNumber(pinNumber), m_type(type)
{
    // shown as small circle
    setPixmap(QPixmap(10, 10));
    QPixmap pix(10, 10);
    pix.fill(Qt::transparent);

    // set power pin on high
    if(m_type == PinItem::Power){
        m_isHigh = true;
    }

    QPainter painter(&pix);
    // Color after Pin-Type
    switch (m_type) {
    case PinType::Ground:
        painter.setBrush(Qt::darkGray);
        break;
    case PinType::Power:
        painter.setBrush(Qt::yellow);
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

    // set power pin on high
    if(m_type == PinItem::Power){
        m_isHigh = true;
    }

    // Color after Pin-Type
    switch (m_type) {
    case PinType::Ground:
        painter.setBrush(Qt::darkGray);
        break;
    case PinType::Power:
        painter.setBrush(Qt::yellow);
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
    if (m_isHigh && this->m_type != Power) return; // allready HIGH and not PowerPin → do nothing

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
        cable->propagateSignal(this);
    }
    // if parent is resistor
    if(auto* parentRes = dynamic_cast<ResistorItem*>(parentItem())){
        parentRes->onPinChanged(this);
    }
    // if parent is led
    if(auto* parentLed = dynamic_cast<LedItem*>(parentItem())){
        parentLed->onPinChanged(this);
    }
    // if parent is button
    if (auto* btn = dynamic_cast<ButtonItem*>(parentItem()))
        btn->onPinChanged(this);
}

void PinItem::setLow()
{
    // Power Pins are always high
    if(m_type == PinType::Power){
        // inform all cables
        for(CableItem* cable : m_cables){
            cable->setHigh();
            cable->propagateSignal(this);
        }
        return;
    }

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
        cable->propagateSignal(this);
    }
    // if parent is resistor
    if(auto* parentRes = dynamic_cast<ResistorItem*>(parentItem())){
        parentRes->onPinChanged(this);
    }
    // if parent is led
    if(auto* parentLed = dynamic_cast<LedItem*>(parentItem())){
        parentLed->onPinChanged(this);
    }
    // if parent is button
    if (auto* btn = dynamic_cast<ButtonItem*>(parentItem()))
        btn->onPinChanged(this);
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
    if(event->button() == Qt::LeftButton){
        // Pin of type Led is not allowed to select
        if(m_type == PinType::Led){
            event->ignore();
            return;
        }

        if(auto simScene = qobject_cast<SimulatorScene*>(scene())){
            simScene->pinClicked(this);
        }
        event->accept();
        return;
    }
    if(event->button() == Qt::RightButton){
        event->ignore();
        return;
    }

    ComponentItem::mousePressEvent(event);
}

void PinItem::setHighlighted(bool on){
    m_highlighted = on;

    QPixmap pix(10,10);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    if (on)
        p.setBrush(Qt::blue);
    else{
        if(this->m_type == Led){
            p.setBrush(Qt::white);
        }
        else if(this->m_type == Ground){
            p.setBrush(Qt::darkGray);
        }
        else if(this->m_type == Power){
            p.setBrush(Qt::yellow);
        }
        else if(this->isHigh()){
            p.setBrush(Qt::green);
        }
        else{
            p.setBrush(Qt::gray);
        }
    }
    p.setPen(Qt::black);
    p.drawEllipse(0,0,10,10);
    p.end();

    setPixmap(pix);
}
