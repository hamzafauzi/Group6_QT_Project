#include "simulatorpanel.h"
#include <QDebug>

SimulatorPanel::SimulatorPanel(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 500);
    loadBackgroundImage();
}

void SimulatorPanel::loadBackgroundImage()
{
    m_arduinoBoardImage = QPixmap(":/resources/images/arduino_board.png");

    if (!m_arduinoBoardImage.isNull()) {
        qDebug() << "Arduino board image loaded successfully";
        qDebug() << "Original image size:" << m_arduinoBoardImage.size();

        // Scale to SMALLER size - adjust these numbers
        m_arduinoBoardImage = m_arduinoBoardImage.scaled(
            450, 350,  // Try smaller: was 650, 500
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        qDebug() << "Scaled image size:" << m_arduinoBoardImage.width() << "x" << m_arduinoBoardImage.height();
    } else {
        qDebug() << "Failed to load Arduino board image";
    }
}

void SimulatorPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Draw dark professional background
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(45, 52, 54));
    gradient.setColorAt(1, QColor(28, 40, 51));
    painter.fillRect(rect(), gradient);

    // Draw border
    painter.setPen(QPen(QColor(99, 110, 114), 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 5, 5);

    // Draw Arduino board LOWER on screen (more space at top)
    if (!m_arduinoBoardImage.isNull()) {
        int x = (width() - m_arduinoBoardImage.width()) / 2;  // Center horizontally
        int y = height() - m_arduinoBoardImage.height() - 30; // Position at bottom with 30px margin

        // Alternative: Use percentage positioning
        // int y = (int)(height() * 0.4);  // Start at 40% down from top

        painter.drawPixmap(x, y, m_arduinoBoardImage);
    } else {
        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect(), Qt::AlignCenter,
                         "Arduino Board\n(Add arduino_board.png to resources/images/)");
    }
}
