#include "ledcomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

LEDComponent::LEDComponent(QWidget *parent)
    : Component(parent)
    , m_isOn(false)
{
    setFixedSize(100, 100);  // Bigger size
    loadImages();
    setStyleSheet("background: transparent;");  // No background
}

LEDComponent::~LEDComponent()
{
}

void LEDComponent::loadImages()
{
    m_ledOnImage = QPixmap(":/resources/images/led_on.png");
    m_ledOffImage = QPixmap(":/resources/images/led_off.png");

    if (!m_ledOnImage.isNull() && !m_ledOffImage.isNull()) {
        // Optional: Extra white removal if PNGs still have issues
        QImage imgOff = m_ledOffImage.toImage().convertToFormat(QImage::Format_ARGB32);
        QImage imgOn = m_ledOnImage.toImage().convertToFormat(QImage::Format_ARGB32);

        // Remove any remaining white pixels
        for (int y = 0; y < imgOff.height(); y++) {
            for (int x = 0; x < imgOff.width(); x++) {
                QColor color(imgOff.pixel(x, y));
                if (color.red() > 240 && color.green() > 240 && color.blue() > 240) {
                    imgOff.setPixel(x, y, qRgba(0, 0, 0, 0));
                }
            }
        }

        for (int y = 0; y < imgOn.height(); y++) {
            for (int x = 0; x < imgOn.width(); x++) {
                QColor color(imgOn.pixel(x, y));
                if (color.red() > 240 && color.green() > 240 && color.blue() > 240) {
                    imgOn.setPixel(x, y, qRgba(0, 0, 0, 0));
                }
            }
        }

        m_ledOffImage = QPixmap::fromImage(imgOff);
        m_ledOnImage = QPixmap::fromImage(imgOn);

        // Scale with high quality
        m_ledOnImage = m_ledOnImage.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_ledOffImage = m_ledOffImage.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        qDebug() << "LED images loaded with extra transparency check";
    } else {
        qDebug() << "Failed to load LED images, using defaults";
        createDefaultImages();
    }
}

void LEDComponent::createDefaultImages()
{
    // Create high-resolution LED OFF image
    m_ledOffImage = QPixmap(120, 120);  // Larger canvas
    m_ledOffImage.fill(Qt::transparent);
    {
        QPainter painter(&m_ledOffImage);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw LED body (darker gray when off)
        QRadialGradient gradient(60, 50, 40);
        gradient.setColorAt(0, QColor(180, 180, 180));
        gradient.setColorAt(0.7, QColor(120, 120, 120));
        gradient.setColorAt(1, QColor(80, 80, 80));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(60, 60, 60), 2));
        painter.drawEllipse(20, 15, 80, 80);

        // Draw LED legs
        painter.setBrush(QColor(169, 169, 169));
        painter.setPen(Qt::NoPen);
        painter.drawRect(35, 95, 10, 25);  // Left leg
        painter.drawRect(75, 95, 10, 25);  // Right leg
    }

    // Create high-resolution LED ON image (bright yellow)
    m_ledOnImage = QPixmap(120, 120);
    m_ledOnImage.fill(Qt::transparent);
    {
        QPainter painterOn(&m_ledOnImage);
        painterOn.setRenderHint(QPainter::Antialiasing);

        // Bright glow effect
        QRadialGradient glowGradient(60, 50, 50);
        glowGradient.setColorAt(0, QColor(255, 255, 100, 200));
        glowGradient.setColorAt(0.5, QColor(255, 220, 0));
        glowGradient.setColorAt(1, QColor(255, 165, 0));

        painterOn.setBrush(glowGradient);
        painterOn.setPen(QPen(QColor(255, 200, 0), 2));
        painterOn.drawEllipse(20, 15, 80, 80);

        // Draw LED legs
        painterOn.setBrush(QColor(169, 169, 169));
        painterOn.setPen(Qt::NoPen);
        painterOn.drawRect(35, 95, 10, 25);
        painterOn.drawRect(75, 95, 10, 25);
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
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Draw LED image WITHOUT pin label
    QPixmap currentImage = m_isOn ? m_ledOnImage : m_ledOffImage;

    if (!currentImage.isNull()) {
        // Draw at full size - no text, no label
        painter.drawPixmap(0, 0, width(), height(), currentImage);
    }

    // REMOVED: Pin number drawing code
}
