#include "buttoncomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QtMath>

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

    // Handle left-click
    if (event->button() == Qt::LeftButton) {
        QPoint clickPos = event->pos();

        // Check if click is on a terminal (highest priority)
        int terminalNumber = 0;
        if (isClickOnTerminal(clickPos, terminalNumber)) {
            qDebug() << "Click on terminal" << terminalNumber;
            emit terminalClicked(this, terminalNumber);
            event->accept();
            return;
        }

        // Check if click is on the green circular button (pressable area)
        if (isClickOnCircle(clickPos)) {
            qDebug() << "Click on green button - activating";

            // Toggle button state
            setPressed(!m_isPressed);

            event->accept();
            return;
        }

        // Otherwise, it's a drag operation
        qDebug() << "Click on box edge - dragging";
        m_dragStartPosition = event->pos();
        m_isDragging = false;
    }

    Component::mousePressEvent(event);
}

bool ButtonComponent::isClickOnCircle(const QPoint &clickPos)
{
    // Button image is 137x122
    // Green circle top-left: (40, 36), size: 57x51
    // Center: (68.5, 61.5), Radius X: 28.5, Radius Y: 25.5

    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    // Green circle center in scaled coordinates
    double centerX = 68.5 * scaleX;
    double centerY = 61.5 * scaleY;

    // Radii in scaled coordinates
    double radiusX = 28.5 * scaleX;
    double radiusY = 25.5 * scaleY;

    // Calculate if click is inside the ellipse
    double dx = (clickPos.x() - centerX) / radiusX;
    double dy = (clickPos.y() - centerY) / radiusY;

    double distance = dx * dx + dy * dy;

    bool isInside = (distance <= 1.0);

    qDebug() << "Click at:" << clickPos
             << "Center:" << QPointF(centerX, centerY)
             << "Distance:" << distance
             << "Inside:" << isInside;

    return isInside;
}

void ButtonComponent::mouseMoveEvent(QMouseEvent *event)
{
    // If mouse moved significantly, it's a drag
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_dragStartPosition;
        if (delta.manhattanLength() > 3) {  // 3 pixel threshold
            m_isDragging = true;
        }
    }

    Component::mouseMoveEvent(event);
}

void ButtonComponent::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    Component::mouseReleaseEvent(event);
}

QPoint ButtonComponent::getTerminal1Pos() const
{
    // Terminal 1 (top-left): X=10, Y=43 in 137x122 image
    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    int termX = (int)(10 * scaleX);
    int termY = (int)(43 * scaleY);

    return mapToParent(QPoint(termX, termY));
}

QPoint ButtonComponent::getTerminal2Pos() const
{
    // Terminal 2 (bottom-left): X=10, Y=78 in 137x122 image
    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    int termX = (int)(10 * scaleX);
    int termY = (int)(78 * scaleY);

    return mapToParent(QPoint(termX, termY));
}

QPoint ButtonComponent::getTerminal3Pos() const
{
    // Terminal 3 (top-right): X=128, Y=42 in 137x122 image
    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    int termX = (int)(128 * scaleX);
    int termY = (int)(42 * scaleY);

    return mapToParent(QPoint(termX, termY));
}

QPoint ButtonComponent::getTerminal4Pos() const
{
    // Terminal 4 (bottom-right): X=128, Y=78 in 137x122 image
    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    int termX = (int)(128 * scaleX);
    int termY = (int)(78 * scaleY);

    return mapToParent(QPoint(termX, termY));
}

bool ButtonComponent::isClickOnTerminal(const QPoint &clickPos, int &terminalNumber)
{
    double scaleX = (double)width() / 137.0;
    double scaleY = (double)height() / 122.0;

    const int clickRadius = 15;  // 15 pixel radius around each terminal

    // Terminal positions in 137x122 image
    struct Terminal {
        int x, y, number;
    };

    Terminal terminals[] = {
        {10, 43, 1},   // Terminal 1 (top-left)
        {10, 78, 2},   // Terminal 2 (bottom-left)
        {128, 42, 3},  // Terminal 3 (top-right)
        {128, 78, 4}   // Terminal 4 (bottom-right)
    };

    // Check each terminal
    for (const Terminal &term : terminals) {
        int termX = (int)(term.x * scaleX);
        int termY = (int)(term.y * scaleY);

        int dist = qSqrt(qPow(clickPos.x() - termX, 2) + qPow(clickPos.y() - termY, 2));

        if (dist <= clickRadius) {
            terminalNumber = term.number;
            qDebug() << "Terminal" << terminalNumber << "clicked at" << clickPos
                     << "terminal pos:" << QPoint(termX, termY) << "distance:" << dist;
            return true;
        }
    }

    return false;
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
