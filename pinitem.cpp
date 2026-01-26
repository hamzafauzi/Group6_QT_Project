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

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant& value){
    if (change == ItemScenePositionHasChanged)
    {
        for (CableItem* cable : m_cables)
            cable->updateLine();
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

ComponentItem* PinItem::parentComponent() const
{
    // The parent is the ComponentItem that owns this pin
    QGraphicsItem* parent = parentItem();
    if (parent) {
        return qgraphicsitem_cast<ComponentItem*>(parent);
    }
    return nullptr;
}

QString PinItem::getPinName() const
{
    ComponentItem* parent = parentComponent();
    if (!parent) {
        return "Unknown";
    }
    
    // Check if parent is LED
    if (LedItem* led = qobject_cast<LedItem*>(parent)) {
        if (this == led->signalPin()) {
            return "Signal Pin";
        } else if (this == led->groundPin()) {
            return "Ground Pin";
        }
    }
    // Check if parent is Resistor
    else if (ResistorItem* res = qobject_cast<ResistorItem*>(parent)) {
        if (this == res->leftPin()) {
            return "Left Pin";
        } else if (this == res->rightPin()) {
            return "Right Pin";
        }
    }
    // Check if parent is Button
    else if (ButtonItem* btn = qobject_cast<ButtonItem*>(parent)) {
        if (this == btn->pin1()) {
            return "Pin 1";
        } else if (this == btn->pin2()) {
            return "Pin 2";
        } else if (this == btn->pin3()) {
            return "Pin 3";
        } else if (this == btn->pin4()) {
            return "Pin 4";
        }
    }
    // Check if parent is Arduino (use dynamic_cast since ArduinoItem doesn't have Q_OBJECT)
    else if (ArduinoItem* arduino = dynamic_cast<ArduinoItem*>(parent)) {
        if (this == arduino->groundPin()) {
            return "Ground Pin";
        } else {
            // Check all Arduino pins
            for (int i = 0; i <= 13; ++i) {
                if (this == arduino->getPin(i)) {
                    return QString("Pin %1").arg(i);
                }
            }
        }
    }
    
    return "Unknown Pin";
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // Only handle left mouse button clicks
    if (event->button() == Qt::LeftButton) {
        QString componentName = "Unknown";
        
        // Try to get component name from Diagram using component ID
        if (m_componentId >= 0 && scene()) {
            Diagram* diagram = Diagram::getDiagramFromScene(scene());
            if (diagram) {
                componentName = diagram->getComponentName(m_componentId);
            }
        }
        
        // If we couldn't get name from Diagram, fall back to type detection
        if (componentName == "Unknown") {
            ComponentItem* parent = parentComponent();
            if (parent) {
                // Determine component type
                if (qobject_cast<LedItem*>(parent)) {
                    componentName = "LED";
                } else if (qobject_cast<ResistorItem*>(parent)) {
                    componentName = "Resistor";
                } else if (qobject_cast<ButtonItem*>(parent)) {
                    componentName = "Button";
                } else if (dynamic_cast<ArduinoItem*>(parent)) {
                    componentName = "Arduino";
                }
            }
        }
        
        // Get pin name
        QString pinName = getPinName();
        
        // Get parent widget from scene for dialog
        QWidget* parentWidget = nullptr;
        if (scene() && scene()->views().size() > 0) {
            parentWidget = scene()->views().first();
        }
        
        // Create and show dialog
        PinInfoDialog* dialog = new PinInfoDialog(parentWidget);
        dialog->setPinInfo(componentName, pinName);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->exec();
        
        // Emit signal for other parts of the system if needed
        emit pinClicked(this);
        
        event->accept();
        return;
    }
    
    // Let parent handle other events
    ComponentItem::mousePressEvent(event);
}

