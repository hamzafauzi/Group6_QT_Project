#include "resistorcomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

ResistorComponent::ResistorComponent(QWidget *parent)
    : Component(parent)
    , m_resistance(220)
{
    setFixedSize(120, 80);  // Bigger
    loadImage();
    setStyleSheet("background: transparent;");
}

ResistorComponent::~ResistorComponent()
{
}

void ResistorComponent::loadImage()
{
    m_resistorImage = QPixmap(":/resources/images/resistor.png");

    if (!m_resistorImage.isNull()) {
        // Remove white background if present
        QImage img = m_resistorImage.toImage().convertToFormat(QImage::Format_ARGB32);

        // Make white pixels transparent
        for (int y = 0; y < img.height(); y++) {
            for (int x = 0; x < img.width(); x++) {
                QColor color(img.pixel(x, y));
                if (color.red() > 240 && color.green() > 240 && color.blue() > 240) {
                    img.setPixel(x, y, qRgba(0, 0, 0, 0));
                }
            }
        }

        m_resistorImage = QPixmap::fromImage(img);

        // Scale to LARGER size with high quality
        m_resistorImage = m_resistorImage.scaled(
            140, 80,  // Larger size (was probably 120, 80)
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation  // High quality scaling
            );

        qDebug() << "Resistor image loaded with transparency";
    } else {
        qDebug() << "Failed to load resistor image, using default";
        createDefaultImage();
    }
}

void ResistorComponent::createDefaultImage()
{
    // Create simple resistor image
    m_resistorImage = QPixmap(70, 30);
    m_resistorImage.fill(Qt::transparent);
    {
        QPainter painter(&m_resistorImage);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw resistor body (rectangle with zigzag pattern)
        painter.setBrush(QColor(210, 180, 140));  // Tan color
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(10, 10, 50, 10);

        // Draw leads (wires on both sides)
        painter.setPen(QPen(Qt::darkGray, 2));
        painter.drawLine(0, 15, 10, 15);  // Left lead
        painter.drawLine(60, 15, 70, 15); // Right lead

        // Draw color bands
        painter.setPen(QPen(Qt::red, 3));
        painter.drawLine(20, 10, 20, 20);
        painter.setPen(QPen(Qt::red, 3));
        painter.drawLine(30, 10, 30, 20);
        painter.setPen(QPen(QColor(139, 69, 19), 3));  // Brown
        painter.drawLine(40, 10, 40, 20);
    }
}

void ResistorComponent::setResistance(int ohms)
{
    m_resistance = ohms;
    update();
}

void ResistorComponent::mousePressEvent(QMouseEvent *event)
{
    // Handle right-click
    if (event->button() == Qt::RightButton) {
        qDebug() << "Resistor: Right-click detected";
        emit componentRightClicked(this);
        event->accept();
        return;
    }

    // Handle other mouse events
    Component::mousePressEvent(event);
}

void ResistorComponent::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);  // High quality

    // Draw resistor image
    if (!m_resistorImage.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), m_resistorImage);
    }

    // Draw resistance value with better text rendering
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Background for text
    QRect textRect(0, height() - 22, width(), 22);
    painter.fillRect(textRect, QColor(0, 0, 0, 150));

    // Draw text
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(textRect, Qt::AlignCenter, QString("%1Ω").arg(m_resistance));
}
