#include "ledcomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

LEDComponent::LEDComponent(QWidget *parent)
    : Component(parent)
    , m_isOn(false)
{
    setFixedSize(60, 60);
    loadImages();
}

LEDComponent::~LEDComponent()
{
}

void LEDComponent::loadImages()
{
    // Try to load from Qt resources
    m_ledOnImage = QPixmap(":/resources/images/led_on.png");
    m_ledOffImage = QPixmap(":/resources/images/led_off.png");

    // Check if images loaded successfully
    if (!m_ledOnImage.isNull() && !m_ledOffImage.isNull()) {
        // Scale images to fit component size
        m_ledOnImage = m_ledOnImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_ledOffImage = m_ledOffImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "LED images loaded successfully from resources";
    } else {
        // Fallback: create default images if loading failed
        qDebug() << "Failed to load LED images, using defaults";
        createDefaultImages();
    }
}

void LEDComponent::createDefaultImages()
{
    // Create simple LED OFF image
    m_ledOffImage = QPixmap(50, 50);
    m_ledOffImage.fill(Qt::transparent);
    {
        QPainter painter(&m_ledOffImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QColor(100, 100, 100));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(5, 5, 40, 40);
    }

    // Create simple LED ON image with glow
    m_ledOnImage = QPixmap(50, 50);
    m_ledOnImage.fill(Qt::transparent);
    {
        QPainter painterOn(&m_ledOnImage);
        painterOn.setRenderHint(QPainter::Antialiasing);

        QRadialGradient gradient(25, 25, 20);
        gradient.setColorAt(0, QColor(255, 255, 100));
        gradient.setColorAt(0.5, QColor(255, 200, 0));
        gradient.setColorAt(1, QColor(200, 150, 0));

        painterOn.setBrush(gradient);
        painterOn.setPen(QPen(QColor(255, 200, 0), 2));
        painterOn.drawEllipse(5, 5, 40, 40);
    }
}

void LEDComponent::setState(bool on)
{
    m_isOn = on;
    update();
}

void LEDComponent::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw border
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Draw LED image
    QPixmap currentImage = m_isOn ? m_ledOnImage : m_ledOffImage;
    painter.drawPixmap(5, 5, currentImage);

    // Draw pin number if assigned
    if (m_pin >= 0) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter,
                         QString("Pin %1").arg(m_pin));
    }
}
