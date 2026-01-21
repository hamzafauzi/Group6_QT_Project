#include "component.h"

Component::Component(QWidget *parent)
    : QWidget(parent)
    , m_pin(-1)
    , m_isDragging(false)
{
    // Ensure transparent background
    setStyleSheet("background: transparent;");
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

Component::~Component()
{
}

void Component::setPosition(const QPoint &pos)
{
    m_position = pos;
    move(pos);
}

void Component::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPosition = event->pos();
        emit componentClicked(this);
    }
    QWidget::mousePressEvent(event);
}

void Component::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->pos() - m_dragStartPosition;
        QPoint newPos = pos() + delta;
        move(newPos);
        m_position = newPos;
        emit componentMoved(this);
    }
    QWidget::mouseMoveEvent(event);
}

void Component::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}
