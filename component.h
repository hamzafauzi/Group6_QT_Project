#ifndef COMPONENT_H
#define COMPONENT_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class Component : public QWidget
{
    Q_OBJECT

public:
    explicit Component(QWidget *parent = nullptr);
    virtual ~Component();

    // Getters and setters
    void setPosition(const QPoint &pos);
    QPoint position() const { return m_position; }

    void setPin(int pin) { m_pin = pin; }
    int pin() const { return m_pin; }

    void setComponentId(int id) { m_componentId = id; }
    int componentId() const { return m_componentId; }

    virtual QString componentType() const = 0;

signals:
    void componentClicked(Component *component);
    void componentMoved(Component *component);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QPoint m_position;
    int m_pin;
    int m_componentId;
    bool m_isDragging;
    QPoint m_dragStartPosition;
};

#endif // COMPONENT_H
