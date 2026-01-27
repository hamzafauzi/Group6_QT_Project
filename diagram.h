#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QObject>
#include <QGraphicsScene>
#include <QList>
#include <QMap>
#include <QPoint>
#include <QString>
#include "componentitem.h"
#include "arduinoitem.h"
#include "leditem.h"
#include "resistoritem.h"
#include "buttonitem.h"
#include "cableitem.h"
#include "pinitem.h"

// Component data structure for serialization
struct ComponentData {
    QString type;           // "LED", "Resistor", "Button"
    int id;                 // Unique component ID
    QPointF position;       // Position on the diagram
    QMap<QString, QVariant> properties;  // Component-specific properties
    
    ComponentData() : id(-1) {}
};

// Connection data structure
struct ConnectionData {
    int id;                 // Unique connection ID
    QString fromComponentType;  // Type of source component
    int fromComponentId;    // ID of source component
    QString fromPinName;    // Name of source pin (e.g., "signalPin", "leftLead", "terminal1")
    QString toComponentType;    // Type of destination component
    int toComponentId;      // ID of destination component
    QString toPinName;      // Name of destination pin
    
    ConnectionData() : id(-1), fromComponentId(-1), toComponentId(-1) {}
};

class Diagram : public QObject
{
    Q_OBJECT

public:
    explicit Diagram(QGraphicsScene *scene, QObject *parent = nullptr);
    ~Diagram();

    // Component management
    int addLED(const QPointF &position);
    int addResistor(const QPointF &position, int resistance = 220);
    int addButton(const QPointF &position);
    
    // Component access
    ComponentItem* getComponent(int id) const;
    ComponentItem* getComponentAt(const QPointF &position) const;
    QList<ComponentItem*> getAllComponents() const;
    
    // Get all pins for a component
    QList<PinItem*> getPinsForComponent(int componentId) const;
    
    // Get component type from component ID
    QString getComponentType(int componentId) const;
    
    // Get component name (e.g., "LED 1", "Button 2")
    QString getComponentName(int componentId) const;

    // Chain of pin names (example: "arduinopin13", "led1pin1", ...)
    QStringList getPinChain() const { return m_pinChain; }
    
    // Connection management
    int addConnection(int fromComponentId, const QString &fromPinName,
                     int toComponentId, const QString &toPinName);
    bool removeConnection(int connectionId);
    QList<ConnectionData> getAllConnections() const;
    QList<ConnectionData> getConnectionsForComponent(int componentId) const;

    // Handle pin click to support two-click connect
    void onPinSelected(PinItem* pin);
    
    // Component removal
    bool removeComponent(int componentId);
    
    // Save/Load functionality
    bool saveToFile(const QString &filename);
    bool loadFromFile(const QString &filename);
    
    // Get Arduino board reference
    ArduinoItem* getArduino() const { return m_arduino; }
    
    // Static method to get Diagram instance from scene
    static Diagram* getDiagramFromScene(QGraphicsScene* scene);

signals:
    void componentAdded(int componentId, const QString &componentType);
    void componentRemoved(int componentId);
    void componentClicked(int componentId);
    void connectionAdded(int connectionId);
    void connectionRemoved(int connectionId);
    void connectionLogged(const QString &message);

private:
    QGraphicsScene *m_scene;
    ArduinoItem *m_arduino;
    
    // Component storage
    QMap<int, ComponentItem*> m_components;
    QMap<int, QString> m_componentNames; // Map component ID to name (e.g., "LED 1", "Button 2")
    QMap<QString, int> m_componentCounters; // Count how many of each type we have
    int m_nextComponentId;
    
    // Chain of pin names in order
    QStringList m_pinChain;
    
    // Connection storage
    QMap<int, CableItem*> m_connections;
    QMap<int, ConnectionData> m_connectionData;
    int m_nextConnectionId;

    // Pending pin selection for two-click connect
    PinItem* m_pendingPin = nullptr;
    
    // Helper methods
    PinItem* getPinForComponent(int componentId, const QString &pinName);
    void updateConnectionsForComponent(int componentId);
    void removeConnectionsForComponent(int componentId);
    QString generateComponentName(const QString &type, int id);
    QString generateConnectionLogMessage(const ConnectionData &conn,PinItem* fromPin,PinItem* toPin);
    QString getPinDisplayName(PinItem* pin, const QString &pinKey);
    
    // Create cable using the old style (manual cable creation)
    CableItem* createCable(PinItem* fromPin, PinItem* toPin);
};

#endif // DIAGRAM_H
