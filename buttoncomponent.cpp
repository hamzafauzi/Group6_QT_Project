#include "buttoncomponent.h"
#include <QPainter>
#include <QFile>

ButtonComponent::ButtonComponent(QWidget *parent)
    : Component(parent)
    , m_isPressed(false)
{
    setFixedSize(60, 60);
    loadImages();
}

ButtonComponent::~ButtonComponent()
{
}

void ButtonComponent::loadImages()
{
    if (QFile::exists(":/resources/images/button_up.png")) {
        m_buttonUpImage = QPixmap(":/resources/images/button_up.png");
        m_buttonDownImage = QPixmap(":/resources/images/button_down.png");

        m_buttonUpImage = m_buttonUpImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_buttonDownImage = m_buttonDownImage.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        createDefaultImages();
    }
}

void ButtonComponent::createDefaultImages()
{
    // Button UP state
    m_buttonUpImage = QPixmap(50, 50);
    m_buttonUpImage.fill(Qt::transparent);
    QPainter painter(&m_buttonUpImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(220, 220, 220));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(5, 5, 40, 40);
    painter.setBrush(QColor(180, 180, 180));
    painter.drawEllipse(15, 15, 20, 20);

    // Button DOWN state
    m_buttonDownImage = QPixmap(50, 50);
    m_buttonDownImage.fill(Qt::transparent);
    QPainter painterDown(&m_buttonDownImage);
    painterDown.setRenderHint(QPainter::Antialiasing);
    painterDown.setBrush(QColor(160, 160, 160));
    painterDown.setPen(QPen(Qt::black, 2));
    painterDown.drawEllipse(5, 5, 40, 40);
    painterDown.setBrush(QColor(120, 120, 120));
    painterDown.drawEllipse(15, 18, 20, 20);
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

    // Draw border
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Draw button image
    QPixmap currentImage = m_isPressed ? m_buttonDownImage : m_buttonUpImage;
    painter.drawPixmap(5, 5, currentImage);

    // Draw pin number if assigned
    if (m_pin >= 0) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter,
                         QString("Pin %1").arg(m_pin));
    }
}
