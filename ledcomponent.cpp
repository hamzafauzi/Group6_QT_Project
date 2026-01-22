#include "ledcomponent.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QMouseEvent>
#include <QRect>

LEDComponent::LEDComponent(QWidget *parent)
    : Component(parent)
    , m_isOn(false)
{
    setFixedSize(150, 150);
    loadImages();
    setStyleSheet("background: transparent;");
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

LEDComponent::~LEDComponent()
{
}

void LEDComponent::loadImages()
{
    m_ledOnImage = QPixmap(":/resources/images/led_on.png");
    m_ledOffImage = QPixmap(":/resources/images/led_off.png");

    if (!m_ledOnImage.isNull() && !m_ledOffImage.isNull()) {
        // Remove white background pixels
        QImage imgOff = m_ledOffImage.toImage().convertToFormat(QImage::Format_ARGB32);
        QImage imgOn = m_ledOnImage.toImage().convertToFormat(QImage::Format_ARGB32);

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

        // Scale to component size
        m_ledOnImage = m_ledOnImage.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_ledOffImage = m_ledOffImage.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        qDebug() << "LED images loaded successfully";
    } else {
        qDebug() << "ERROR: Failed to load LED images!";
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

    // Draw LED image only
    QPixmap currentImage = m_isOn ? m_ledOnImage : m_ledOffImage;

    if (!currentImage.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), currentImage);
    }
}

QPoint LEDComponent::getPositiveLegPos() const
{
    // LONG leg (positive) is on the LEFT at: X=282, Y=427

    double scaleX = (double)width() / 600.0;
    double scaleY = (double)height() / 500.0;

    int legX = (int)(282 * scaleX);  // LEFT leg
    int legY = (int)(427 * scaleY);

    qDebug() << "getPositiveLegPos (LONG/LEFT): legX=" << legX << "legY=" << legY;

    QPoint parentPos = mapToParent(QPoint(legX, legY));
    qDebug() << "getPositiveLegPos: parentPos=" << parentPos;

    return parentPos;
}

QPoint LEDComponent::getNegativeLegPos() const
{
    // SHORT leg (negative) is on the RIGHT at: X=319, Y=360

    double scaleX = (double)width() / 600.0;
    double scaleY = (double)height() / 500.0;

    int legX = (int)(319 * scaleX);  // RIGHT leg
    int legY = (int)(360 * scaleY);

    qDebug() << "getNegativeLegPos (SHORT/RIGHT): legX=" << legX << "legY=" << legY;

    QPoint parentPos = mapToParent(QPoint(legX, legY));
    qDebug() << "getNegativeLegPos: parentPos=" << parentPos;

    return parentPos;
}

bool LEDComponent::isClickOnLeg(const QPoint &clickPos, bool &isPositive)
{
    double scaleX = (double)width() / 600.0;
    double scaleY = (double)height() / 500.0;

    const int legWidth  = 20;
    const int legHeight = 40;   // IMPORTANT: fixed height

    // LONG LEG (POSITIVE) - LEFT
    int longLegX = (int)(282 * scaleX) - legWidth / 2;
    int longLegY = (int)(427 * scaleY);
    QRect positiveLegArea(longLegX, longLegY, legWidth, legHeight);

    if (positiveLegArea.contains(clickPos)) {
        isPositive = true;
        qDebug() << "Click detected on POSITIVE leg";
        return true;
    }

    // SHORT LEG (NEGATIVE) - RIGHT
    int shortLegX = (int)(319 * scaleX) - legWidth / 2;
    int shortLegY = (int)(360 * scaleY);
    QRect negativeLegArea(shortLegX, shortLegY, legWidth, legHeight);

    if (negativeLegArea.contains(clickPos)) {
        isPositive = false;
        qDebug() << "Click detected on NEGATIVE leg";
        return true;
    }

    return false;
}


void LEDComponent::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "=== LED mousePressEvent ===";
    qDebug() << "Button:" << event->button();
    qDebug() << "Position:" << event->pos();

    // Handle right-click first
    if (event->button() == Qt::RightButton) {
        qDebug() << "LED: Right-click detected, emitting componentRightClicked";
        emit componentRightClicked(this);
        event->accept();
        return;
    }

    // Handle left-click on legs
    if (event->button() == Qt::LeftButton) {
        bool isPositive;

        if (isClickOnLeg(event->pos(), isPositive)) {
            qDebug() << "LED leg clicked!" << (isPositive ? "POSITIVE (Long)" : "NEGATIVE (Short)");
            emit legClicked(this, isPositive);
            qDebug() << "legClicked signal emitted";
            return;
        }
    }

    qDebug() << "Not on leg, calling Component::mousePressEvent";
    Component::mousePressEvent(event);
}
