#include "diagram.h"
#include "simulatorscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>

Diagram::Diagram(QGraphicsScene *scene, QObject *parent)
    : QObject(parent)
    , m_scene(scene)
    , m_arduino(nullptr)
    , m_nextComponentId(1)
    , m_nextConnectionId(1)
{
    // Initialize component counters
    m_componentCounters["LED"] = 0;
    m_componentCounters["Resistor"] = 0;
    m_componentCounters["Button"] = 0;
    
    // Create Arduino board if scene is provided
    if (m_scene) {
        m_arduino = new ArduinoItem();
        m_scene->addItem(m_arduino);
    }
}

Diagram::~Diagram()
{
    // Components and connections will be deleted by the scene
    // Just clear our maps
    m_components.clear();
    m_connections.clear();
}

int Diagram::addLED(const QPointF &position)
{
    if (!m_scene) return -1;
    
    LedItem *led = new LedItem();
    led->setPos(position);
    m_scene->addItem(led);
    
    int id = m_nextComponentId++;
    m_components[id] = led;
    
    // Create component name
    m_componentCounters["LED"]++;
    QString componentName = QString("LED %1").arg(m_componentCounters["LED"]);
    m_componentNames[id] = componentName;
    
    // Store component ID in pins so they know which component they belong to
    led->signalPin()->setComponentId(id);
    led->groundPin()->setComponentId(id);
    
    // Make component clickable
    led->setFlag(QGraphicsItem::ItemIsSelectable, true);
    
    // Create default cables using the old style
    if (m_arduino) {
        // Connect LED ground to Arduino ground (old style)
        CableItem* groundCable = createCable(led->groundPin(), m_arduino->groundPin());
        
        if (groundCable) {
            // Store the connection
            int connId = m_nextConnectionId++;
            m_connections[connId] = groundCable;
            ConnectionData connData;
            connData.id = connId;
            connData.fromComponentType = "LED";
            connData.fromComponentId = id;
            connData.fromPinName = "groundPin";
            connData.toComponentType = "Arduino";
            connData.toComponentId = 0;
            connData.toPinName = "ground";
            m_connectionData[connId] = connData;
        }
    }
    
    emit componentAdded(id, "LED");
    return id;
}

int Diagram::addResistor(const QPointF &position, int resistance)
{
    if (!m_scene) return -1;
    
    ResistorItem *resistor = new ResistorItem();
    resistor->setPos(position);
    resistor->setResistance(resistance);
    m_scene->addItem(resistor);
    
    int id = m_nextComponentId++;
    m_components[id] = resistor;
    
    // Create component name
    m_componentCounters["Resistor"]++;
    QString componentName = QString("Resistor %1").arg(m_componentCounters["Resistor"]);
    m_componentNames[id] = componentName;
    
    // Store component ID in pins so they know which component they belong to
    resistor->leftPin()->setComponentId(id);
    resistor->rightPin()->setComponentId(id);
    
    // Make component clickable
    resistor->setFlag(QGraphicsItem::ItemIsSelectable, true);
    
    // All pins know their parent component through parentItem()
    // The schematic is fully tracked in diagram.cpp
    
    emit componentAdded(id, "Resistor");
    return id;
}

int Diagram::addButton(const QPointF &position)
{
    if (!m_scene) return -1;
    
    ButtonItem *button = new ButtonItem();
    button->setPos(position);
    m_scene->addItem(button);
    
    int id = m_nextComponentId++;
    m_components[id] = button;
    
    // Create component name
    m_componentCounters["Button"]++;
    QString componentName = QString("Button %1").arg(m_componentCounters["Button"]);
    m_componentNames[id] = componentName;
    
    // Store component ID in pins so they know which component they belong to
    button->pin1()->setComponentId(id);
    button->pin2()->setComponentId(id);
    button->pin3()->setComponentId(id);
    button->pin4()->setComponentId(id);
    
    // Make component clickable
    button->setFlag(QGraphicsItem::ItemIsSelectable, true);
    
    // All pins know their parent component through parentItem()
    // The schematic is fully tracked in diagram.cpp
    
    emit componentAdded(id, "Button");
    return id;
}

ComponentItem* Diagram::getComponent(int id) const
{
    return m_components.value(id, nullptr);
}

ComponentItem* Diagram::getComponentAt(const QPointF &position) const
{
    if (!m_scene) return nullptr;
    
    QList<QGraphicsItem*> items = m_scene->items(position);
    for (QGraphicsItem* item : items) {
        ComponentItem* component = qgraphicsitem_cast<ComponentItem*>(item);
        if (component && component != m_arduino) {
            // Find the component ID
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                if (it.value() == component) {
                    return component;
                }
            }
        }
    }
    return nullptr;
}

QList<ComponentItem*> Diagram::getAllComponents() const
{
    return m_components.values();
}

QList<PinItem*> Diagram::getPinsForComponent(int componentId) const
{
    QList<PinItem*> pins;
    ComponentItem* component = getComponent(componentId);
    
    if (!component) {
        // Check if it's Arduino
        if (componentId == 0 && m_arduino) {
            // Get all Arduino pins
            for (int i = 0; i <= 13; ++i) {
                PinItem* pin = m_arduino->getPin(i);
                if (pin) {
                    pins.append(pin);
                }
            }
            PinItem* ground = m_arduino->groundPin();
            if (ground) {
                pins.append(ground);
            }
        }
        return pins;
    }
    
    // Get pins based on component type
    if (LedItem* led = qobject_cast<LedItem*>(component)) {
        pins.append(led->signalPin());
        pins.append(led->groundPin());
    } else if (ResistorItem* res = qobject_cast<ResistorItem*>(component)) {
        pins.append(res->leftPin());
        pins.append(res->rightPin());
    } else if (ButtonItem* btn = qobject_cast<ButtonItem*>(component)) {
        pins.append(btn->pin1());
        pins.append(btn->pin2());
        pins.append(btn->pin3());
        pins.append(btn->pin4());
    }
    
    return pins;
}

QString Diagram::getComponentType(int componentId) const
{
    ComponentItem* component = getComponent(componentId);
    
    if (!component) {
        if (componentId == 0 && m_arduino) {
            return "Arduino";
        }
        return QString();
    }
    
    if (qobject_cast<LedItem*>(component)) {
        return "LED";
    } else if (qobject_cast<ResistorItem*>(component)) {
        return "Resistor";
    } else if (qobject_cast<ButtonItem*>(component)) {
        return "Button";
    }
    
    return QString();
}

QString Diagram::getComponentName(int componentId) const
{
    // Check if it's Arduino
    if (componentId == 0 && m_arduino) {
        return "Arduino";
    }
    
    // Get component name from map
    if (m_componentNames.contains(componentId)) {
        return m_componentNames.value(componentId);
    }
    
    // Fallback to type if name not found
    return getComponentType(componentId);
}

int Diagram::addConnection(int fromComponentId, const QString &fromPinName,
                          int toComponentId, const QString &toPinName)
{
    if (!m_scene) return -1;
    
    PinItem* fromPin = getPinForComponent(fromComponentId, fromPinName);
    PinItem* toPin = getPinForComponent(toComponentId, toPinName);
    
    if (!fromPin || !toPin) {
        qDebug() << "Failed to create connection: invalid pins";
        return -1;
    }
    
    // Create cable using the old style
    CableItem* cable = createCable(fromPin, toPin);
    
    if (!cable) {
        qDebug() << "Failed to create cable";
        return -1;
    }
    
    int id = m_nextConnectionId++;
    m_connections[id] = cable;
    
    ConnectionData connData;
    connData.id = id;
    
    // Determine component types
    ComponentItem* fromComp = getComponent(fromComponentId);
    ComponentItem* toComp = getComponent(toComponentId);
    
    // Check if it's Arduino (Arduino is not in m_components, it's stored separately)
    if (fromComponentId == 0 && m_arduino) {
        connData.fromComponentType = "Arduino";
    } else if (fromComp) {
        if (qobject_cast<LedItem*>(fromComp)) connData.fromComponentType = "LED";
        else if (qobject_cast<ResistorItem*>(fromComp)) connData.fromComponentType = "Resistor";
        else if (qobject_cast<ButtonItem*>(fromComp)) connData.fromComponentType = "Button";
    }
    
    if (toComponentId == 0 && m_arduino) {
        connData.toComponentType = "Arduino";
    } else if (toComp) {
        if (qobject_cast<LedItem*>(toComp)) connData.toComponentType = "LED";
        else if (qobject_cast<ResistorItem*>(toComp)) connData.toComponentType = "Resistor";
        else if (qobject_cast<ButtonItem*>(toComp)) connData.toComponentType = "Button";
    }
    
    connData.fromComponentId = fromComponentId;
    connData.fromPinName = fromPinName;
    connData.toComponentId = toComponentId;
    connData.toPinName = toPinName;
    
    m_connectionData[id] = connData;
    
    // Note: createCable() already added the cable to pins and scene
    // So we don't need to do it again here
    
    emit connectionAdded(id);
    return id;
}

bool Diagram::removeConnection(int connectionId)
{
    if (!m_connections.contains(connectionId)) {
        return false;
    }
    
    CableItem* cable = m_connections[connectionId];
    m_scene->removeItem(cable);
    delete cable;
    
    m_connections.remove(connectionId);
    m_connectionData.remove(connectionId);
    
    emit connectionRemoved(connectionId);
    return true;
}

QList<ConnectionData> Diagram::getAllConnections() const
{
    return m_connectionData.values();
}

QList<ConnectionData> Diagram::getConnectionsForComponent(int componentId) const
{
    QList<ConnectionData> result;
    for (const ConnectionData& conn : m_connectionData.values()) {
        if (conn.fromComponentId == componentId || conn.toComponentId == componentId) {
            result.append(conn);
        }
    }
    return result;
}

bool Diagram::removeComponent(int componentId)
{
    if (!m_components.contains(componentId)) {
        return false;
    }
    
    ComponentItem* component = m_components[componentId];
    
    // Remove all connections for this component
    removeConnectionsForComponent(componentId);
    
    // Remove from scene
    m_scene->removeItem(component);
    delete component;
    
    // Remove from maps
    m_components.remove(componentId);
    m_componentNames.remove(componentId);
    
    emit componentRemoved(componentId);
    return true;
}

bool Diagram::saveToFile(const QString &filename)
{
    QJsonObject root;
    
    // Save components
    QJsonArray componentsArray;
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        ComponentItem* comp = it.value();
        ComponentData compData;
        compData.id = it.key();
        compData.position = comp->pos();
        
        // Determine type and properties
        if (LedItem* led = qobject_cast<LedItem*>(comp)) {
            compData.type = "LED";
        } else if (ResistorItem* res = qobject_cast<ResistorItem*>(comp)) {
            compData.type = "Resistor";
            compData.properties["resistance"] = res->resistance();
        } else if (ButtonItem* btn = qobject_cast<ButtonItem*>(comp)) {
            compData.type = "Button";
        }
        
        QJsonObject compObj;
        compObj["id"] = compData.id;
        compObj["type"] = compData.type;
        compObj["x"] = compData.position.x();
        compObj["y"] = compData.position.y();
        
        if (!compData.properties.isEmpty()) {
            QJsonObject propsObj;
            for (auto propIt = compData.properties.begin(); propIt != compData.properties.end(); ++propIt) {
                propsObj[propIt.key()] = QJsonValue::fromVariant(propIt.value());
            }
            compObj["properties"] = propsObj;
        }
        
        componentsArray.append(compObj);
    }
    root["components"] = componentsArray;
    
    // Save connections
    QJsonArray connectionsArray;
    for (const ConnectionData& conn : m_connectionData.values()) {
        QJsonObject connObj;
        connObj["id"] = conn.id;
        connObj["fromComponentId"] = conn.fromComponentId;
        connObj["fromComponentType"] = conn.fromComponentType;
        connObj["fromPinName"] = conn.fromPinName;
        connObj["toComponentId"] = conn.toComponentId;
        connObj["toComponentType"] = conn.toComponentType;
        connObj["toPinName"] = conn.toPinName;
        connectionsArray.append(connObj);
    }
    root["connections"] = connectionsArray;
    
    // Write to file
    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool Diagram::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Clear existing components and connections
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        m_scene->removeItem(it.value());
        delete it.value();
    }
    m_components.clear();
    
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        m_scene->removeItem(it.value());
        delete it.value();
    }
    m_connections.clear();
    m_connectionData.clear();
    
    // Load components
    QJsonArray componentsArray = root["components"].toArray();
    QMap<int, int> oldIdToNewId;  // Map old IDs to new IDs
    
    for (const QJsonValue& value : componentsArray) {
        QJsonObject compObj = value.toObject();
        QString type = compObj["type"].toString();
        QPointF position(compObj["x"].toDouble(), compObj["y"].toDouble());
        int oldId = compObj["id"].toInt();
        
        int newId = -1;
        if (type == "LED") {
            newId = addLED(position);
        } else if (type == "Resistor") {
            int resistance = 220;
            if (compObj.contains("properties")) {
                QJsonObject props = compObj["properties"].toObject();
                if (props.contains("resistance")) {
                    resistance = props["resistance"].toInt();
                }
            }
            newId = addResistor(position, resistance);
        } else if (type == "Button") {
            newId = addButton(position);
        }
        
        if (newId != -1) {
            oldIdToNewId[oldId] = newId;
        }
    }
    
    // Load connections
    QJsonArray connectionsArray = root["connections"].toArray();
    for (const QJsonValue& value : connectionsArray) {
        QJsonObject connObj = value.toObject();
        int oldFromId = connObj["fromComponentId"].toInt();
        int oldToId = connObj["toComponentId"].toInt();
        QString fromPinName = connObj["fromPinName"].toString();
        QString toPinName = connObj["toPinName"].toString();
        
        int newFromId = oldIdToNewId.value(oldFromId, -1);
        int newToId = oldIdToNewId.value(oldToId, -1);
        
        if (newFromId != -1 && newToId != -1) {
            addConnection(newFromId, fromPinName, newToId, toPinName);
        }
    }
    
    return true;
}

PinItem* Diagram::getPinForComponent(int componentId, const QString &pinName)
{
    ComponentItem* component = getComponent(componentId);
    if (!component) {
        // Check if it's Arduino
        if (componentId == 0 && m_arduino) {
            if (pinName.startsWith("pin")) {
                int pinNum = pinName.mid(3).toInt();
                return m_arduino->getPin(pinNum);
            } else if (pinName == "ground") {
                return m_arduino->groundPin();
            }
        }
        return nullptr;
    }
    
    if (LedItem* led = qobject_cast<LedItem*>(component)) {
        if (pinName == "signalPin") return led->signalPin();
        if (pinName == "groundPin") return led->groundPin();
    } else if (ResistorItem* res = qobject_cast<ResistorItem*>(component)) {
        if (pinName == "leftPin") return res->leftPin();
        if (pinName == "rightPin") return res->rightPin();
    } else if (ButtonItem* btn = qobject_cast<ButtonItem*>(component)) {
        if (pinName == "pin1") return btn->pin1();
        if (pinName == "pin2") return btn->pin2();
        if (pinName == "pin3") return btn->pin3();
        if (pinName == "pin4") return btn->pin4();
    }
    
    return nullptr;
}

void Diagram::updateConnectionsForComponent(int componentId)
{
    // Update visual representation of connections
    QList<ConnectionData> connections = getConnectionsForComponent(componentId);
    for (const ConnectionData& conn : connections) {
        if (m_connections.contains(conn.id)) {
            m_connections[conn.id]->updateLine();
        }
    }
}

void Diagram::removeConnectionsForComponent(int componentId)
{
    QList<int> connectionsToRemove;
    for (auto it = m_connectionData.begin(); it != m_connectionData.end(); ++it) {
        if (it.value().fromComponentId == componentId || it.value().toComponentId == componentId) {
            connectionsToRemove.append(it.key());
        }
    }
    
    for (int connId : connectionsToRemove) {
        removeConnection(connId);
    }
}

QString Diagram::generateComponentName(const QString &type, int id)
{
    return QString("%1_%2").arg(type).arg(id);
}

CableItem* Diagram::createCable(PinItem* fromPin, PinItem* toPin)
{
    if (!m_scene || !fromPin || !toPin) {
        return nullptr;
    }
    
    // Create cable using the old style
    CableItem* cable = new CableItem(fromPin, toPin);
    
    // Pins must know the cable (old style)
    fromPin->addCable(cable);
    toPin->addCable(cable);
    
    // Add to scene
    m_scene->addItem(cable);
    
    return cable;
}

Diagram* Diagram::getDiagramFromScene(QGraphicsScene* scene)
{
    if (!scene) {
        return nullptr;
    }
    
    // Try to get Diagram from scene property first
    QVariant diagramVar = scene->property("DiagramPointer");
    if (diagramVar.isValid()) {
        Diagram* diagram = diagramVar.value<Diagram*>();
        if (diagram) {
            return diagram;
        }
    }
    
    // Fallback: search for Diagram object in scene's parent hierarchy
    QObject* parent = scene->parent();
    while (parent) {
        Diagram* diagram = qobject_cast<Diagram*>(parent);
        if (diagram) {
            return diagram;
        }
        parent = parent->parent();
    }
    
    return nullptr;
}
