#include "buttoncomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

ButtonComponent::ButtonComponent(QWidget *parent)
    : Component(parent)
    , m_isPressed(false)
{
    setFixedSize(80, 100);  // Bigger
    loadImages();
    setStyleSheet("background: transparent;");
}

ButtonComponent::~ButtonComponent()
{
}

void ButtonComponent::loadImages()
{
    m_buttonUpImage = QPixmap(":/resources/images/button_up.png");
    m_buttonDownImage = QPixmap(":/resources/images/button_up.png");

    if (!m_buttonUpImage.isNull()) {
        // Remove white background from UP image
        QImage imgUp = m_buttonUpImage.toImage().convertToFormat(QImage::Format_ARGB32);

        for (int y = 0; y < imgUp.height(); y++) {
            for (int x = 0; x < imgUp.width(); x++) {
                QColor color(imgUp.pixel(x, y));
                if (color.red() > 240 && color.green() > 240 && color.blue() > 240) {
                    imgUp.setPixel(x, y, qRgba(0, 0, 0, 0));
                }
            }
        }

        m_buttonUpImage = QPixmap::fromImage(imgUp);

        // Create DOWN image (darker version)
        QImage imgDown = imgUp.copy();

        // Darken the image for pressed state
        for (int y = 0; y < imgDown.height(); y++) {
            for (int x = 0; x < imgDown.width(); x++) {
                QColor color(imgDown.pixel(x, y));
                if (color.alpha() > 0) {  // Only darken non-transparent pixels
                    // Reduce brightness by 30%
                    int r = (int)(color.red() * 0.7);
                    int g = (int)(color.green() * 0.7);
                    int b = (int)(color.blue() * 0.7);
                    imgDown.setPixel(x, y, qRgba(r, g, b, color.alpha()));
                }
            }
        }

        m_buttonDownImage = QPixmap::fromImage(imgDown);

        // Scale both
        m_buttonUpImage = m_buttonUpImage.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_buttonDownImage = m_buttonDownImage.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        qDebug() << "Button images loaded (DOWN state darkened)";
    } else {
        createDefaultImages();
    }
}

void ButtonComponent::createDefaultImages()
{
    // Button UP state
    m_buttonUpImage = QPixmap(50, 50);
    m_buttonUpImage.fill(Qt::transparent);
    {
        QPainter painter(&m_buttonUpImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QColor(220, 220, 220));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(5, 5, 40, 40);
        painter.setBrush(QColor(180, 180, 180));
        painter.drawEllipse(15, 15, 20, 20);
    }

    // Button DOWN state
    m_buttonDownImage = QPixmap(50, 50);
    m_buttonDownImage.fill(Qt::transparent);
    {
        QPainter painterDown(&m_buttonDownImage);
        painterDown.setRenderHint(QPainter::Antialiasing);
        painterDown.setBrush(QColor(160, 160, 160));
        painterDown.setPen(QPen(Qt::black, 2));
        painterDown.drawEllipse(5, 5, 40, 40);
        painterDown.setBrush(QColor(120, 120, 120));
        painterDown.drawEllipse(15, 18, 20, 20);
    }
}

void ButtonComponent::setPressed(bool pressed)
{
    m_isPressed = pressed;
    update();

    if (pressed) {
        emit buttonPressed();
    } else {
        emit buttonReleased();
    }
}

void ButtonComponent::mousePressEvent(QMouseEvent *event)
{
    // Handle right-click first
    if (event->button() == Qt::RightButton) {
        qDebug() << "Button: Right-click detected";
        emit componentRightClicked(this);
        event->accept();
        return;
    }

    // Handle left-click for button press
    if (event->button() == Qt::LeftButton) {
        setPressed(true);
    }

    Component::mousePressEvent(event);
}

void ButtonComponent::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setPressed(false);
    }
    Component::mouseReleaseEvent(event);
}

void ButtonComponent::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw button image
    QPixmap currentImage = m_isPressed ? m_buttonDownImage : m_buttonUpImage;
    painter.drawPixmap(0, 0, width(), height() - 20, currentImage);

    // Draw pin number if assigned
    if (m_pin >= 0) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(0, height() - 15, width(), 15, Qt::AlignCenter,
                         QString("Pin %1").arg(m_pin));
    }
}
