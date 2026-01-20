#include "ledcomponent.h"
#include <QPainter>
#include <QFile>

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
    // Try to load from resources
    if (QFile::exists(":/resources/images/led_on.png")) {
        m_ledOnImage = QPixmap(":/resources/images/led_on.png");
        m_ledOffImage = QPixmap(":/resources/images/led_off.png");

        // Scale images to fit component size
        m_ledOnImage = m_ledOnImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_ledOffImage = m_ledOffImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // Create default images if resources not available
        createDefaultImages();
    }
}

void LEDComponent::createDefaultImages()
{
    // Create simple LED OFF image programmatically
    m_ledOffImage = QPixmap(50, 50);
    m_ledOffImage.fill(Qt::transparent);
    QPainter painter(&m_ledOffImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(100, 100, 100));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(5, 5, 40, 40);

    // Create simple LED ON image programmatically
    m_ledOnImage = QPixmap(50, 50);
    m_ledOnImage.fill(Qt::transparent);
    QPainter painterOn(&m_ledOnImage);
    painterOn.setRenderHint(QPainter::Antialiasing);

    // Glowing effect for ON state
    QRadialGradient gradient(25, 25, 25);
    gradient.setColorAt(0, QColor(255, 255, 100));
    gradient.setColorAt(0.5, QColor(255, 200, 0));
    gradient.setColorAt(1, QColor(200, 150, 0));

    painterOn.setBrush(gradient);
    painterOn.setPen(QPen(QColor(255, 200, 0), 2));
    painterOn.drawEllipse(5, 5, 40, 40);
}

void LEDComponent::setState(bool on)
{
    m_isOn = on;
    update(); // Trigger repaint
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
