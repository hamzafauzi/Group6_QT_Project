#ifndef PINITEM_H
#define PINITEM_H

#pragma once
#include "componentitem.h"
#include <QBrush>

class CableItem;

class PinItem : public ComponentItem
{
    Q_OBJECT
public:
    enum PinType { Digital, Analog, Led, Ground, Power };

    // for digital / analog
    PinItem(int pinNumber, PinType type = Digital, QGraphicsItem* parent = nullptr);

    // for GND / Power
    PinItem(PinType, QGraphicsItem* parent = nullptr);

    void updateState() override;   // Pflicht für ComponentItem

    // Signals
    void setHigh();
    void setLow();
    bool isHigh() const {return m_isHigh;}

    // Cable management
    void addCable(CableItem* cable);

    // Getter
    int pinNumber() const { return m_pinNumber; }
    PinType pinType() const {return m_type;}
    
    // Get parent component
    ComponentItem* parentComponent() const;
    
    // Get pin name based on parent component type
    QString getPinName() const;
    
    // Get and set component ID
    int getComponentId() const { return m_componentId; }
    void setComponentId(int id) { m_componentId = id; }
    
    // Unique pin id string (example: "resistor1pin1")
    QString uniqueName() const { return m_uniqueName; }
    void setUniqueName(const QString &name) { m_uniqueName = name; }

    // Internal pin key used for connections (signalPin, groundPin, leftPin, etc.)
    QString pinKey() const { return m_pinKey; }
    void setPinKey(const QString &key) { m_pinKey = key; }

signals:
    void stateChanged(bool high); // High / Low Signal
    void pinClicked(PinItem* pin); // Signal emitted when pin is clicked

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int m_pinNumber;
    PinType m_type;
    bool m_isHigh = false;
    int m_componentId = -1; // ID of the component this pin belongs to
    QString m_uniqueName;   // Unique pin id string
    QString m_pinKey;       // Internal pin key for connection mapping

    QSet<CableItem*> m_cables;
};


#endif // PINITEM_H
