#include "wire.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QtMath>

Wire::Wire(QWidget *parent)
    : QWidget(parent)
{
    // Default wire appearance
    m_pen = QPen(Qt::black, 3);
    m_pen.setCapStyle(Qt::RoundCap);

    // Make background transparent
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);  // Wires don't block clicks
    setAutoFillBackground(false);
}

void Wire::setStartPoint(const QPoint &point)
{
    m_startPoint = point;
    updateGeometry();
}

void Wire::setEndPoint(const QPoint &point)
{
    m_endPoint = point;
    updateGeometry();
}

void Wire::setColor(const QColor &color)
{
    m_pen.setColor(color);
    update();
}

void Wire::setThickness(int thickness)
{
    m_pen.setWidth(thickness);
    update();
}

void Wire::updateGeometry()
{
    if (m_startPoint.isNull() || m_endPoint.isNull()) {
        return;
    }

    // Calculate bounding rectangle for the wire
    int minX = qMin(m_startPoint.x(), m_endPoint.x()) - 10;
    int minY = qMin(m_startPoint.y(), m_endPoint.y()) - 10;
    int maxX = qMax(m_startPoint.x(), m_endPoint.x()) + 10;
    int maxY = qMax(m_startPoint.y(), m_endPoint.y()) + 10;

    setGeometry(minX, minY, maxX - minX, maxY - minY);
    update();
}

void Wire::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_startPoint.isNull() || m_endPoint.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Convert global points to widget-local coordinates
    QPoint localStart = m_startPoint - pos();
    QPoint localEnd = m_endPoint - pos();

    // Draw the wire
    painter.setPen(m_pen);
    painter.drawLine(localStart, localEnd);
}
