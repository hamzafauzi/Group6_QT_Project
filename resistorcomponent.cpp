#include "resistorcomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

ResistorComponent::ResistorComponent(QWidget *parent)
    : Component(parent)
    , m_resistance(220)  // Default 220 ohms
{
    setFixedSize(80, 40);
    loadImage();
}

ResistorComponent::~ResistorComponent()
{
}

void ResistorComponent::loadImage()
{
    // Try to load from Qt resources
    m_resistorImage = QPixmap(":/resources/images/resistor.png");

    // Check if image loaded successfully
    if (!m_resistorImage.isNull()) {
        // Scale image to fit component size
        m_resistorImage = m_resistorImage.scaled(70, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Resistor image loaded successfully from resources";
    } else {
        // Fallback: create default image if loading failed
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

void ResistorComponent::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw border
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Draw resistor image
    painter.drawPixmap(5, 5, m_resistorImage);

    // Draw resistance value
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter,
                     QString("%1Ω").arg(m_resistance));
}
