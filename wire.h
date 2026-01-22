#ifndef WIRE_H
#define WIRE_H

#include <QWidget>
#include <QPoint>
#include <QPen>

class Wire : public QWidget
{
    Q_OBJECT

public:
    explicit Wire(QWidget *parent = nullptr);

    void setStartPoint(const QPoint &point);
    void setEndPoint(const QPoint &point);

    void setColor(const QColor &color);
    void setThickness(int thickness);

    QPoint startPoint() const { return m_startPoint; }
    QPoint endPoint() const { return m_endPoint; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPoint m_startPoint;
    QPoint m_endPoint;
    QPen m_pen;

    void updateGeometry();
};

#endif // WIRE_H
