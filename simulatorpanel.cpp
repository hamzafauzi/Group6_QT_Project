#include "simulatorpanel.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

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
        qDebug() << "Original image size:" << m_arduinoBoardImage.size();  // Should show 1280x907

        // Scale to fit panel
        m_arduinoBoardImage = m_arduinoBoardImage.scaled(
            450, 350,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        qDebug() << "Scaled image size:" << m_arduinoBoardImage.width() << "x" << m_arduinoBoardImage.height();
    } else {
        qDebug() << "Failed to load Arduino board image";
    }
}

QPoint SimulatorPanel::getPinPosition(int pin) const
{
    if (m_arduinoBoardImage.isNull()) {
        return QPoint(0, 0);
    }

    // Calculate board position (must match paintEvent)
    int boardX = (width() - m_arduinoBoardImage.width()) / 2;
    int boardY = height() - m_arduinoBoardImage.height() - 30;

    // Your Arduino board image is 1280 x 907
    double scaleX = (double)m_arduinoBoardImage.width() / 1280.0;
    double scaleY = (double)m_arduinoBoardImage.height() / 907.0;

    double baseX = 0;
    double baseY = 0;

    // DIGITAL PINS (Top Row) - Your exact measurements
    switch(pin) {
    case 13: baseX = 603.0;  baseY = 45.0; break;
    case 12: baseX = 648.0;  baseY = 45.0; break;
    case 11: baseX = 688.5;  baseY = 45.0; break;
    case 10: baseX = 730.0;  baseY = 45.0; break;
    case 9:  baseX = 772.5;  baseY = 45.0; break;
    case 8:  baseX = 817.5;  baseY = 45.0; break;
    case 7:  baseX = 889.5;  baseY = 45.0; break;
    case 6:  baseX = 933.0;  baseY = 45.0; break;
    case 5:  baseX = 978.0;  baseY = 45.0; break;
    case 4:  baseX = 1017.0; baseY = 45.0; break;
    case 3:  baseX = 1059.0; baseY = 45.0; break;
    case 2:  baseX = 1102.5; baseY = 45.0; break;
    case 1:  baseX = 1145.5; baseY = 45.0; break;  // Estimated
    case 0:  baseX = 1188.5; baseY = 45.0; break;  // Estimated

    // ANALOG PINS (Bottom Row)
    case 14: baseX = 973.5;  baseY = 859.5; break;  // A0
    case 15: baseX = 1015.5; baseY = 859.5; break;  // A1
    case 16: baseX = 1063.5; baseY = 859.5; break;  // A2
    case 17: baseX = 1111.5; baseY = 859.5; break;  // A3
    case 18: baseX = 1146.0; baseY = 859.5; break;  // A4
    case 19: baseX = 1192.5; baseY = 859.5; break;  // A5

    // POWER PINS (Bottom Left)
    case 20: baseX = 673.5;  baseY = 859.5; break;  // RESET
    case 21: baseX = 712.5;  baseY = 859.5; break;  // 3.3V
    case 22: baseX = 759.0;  baseY = 859.5; break;  // 5V
    case 23: baseX = 805.5;  baseY = 859.5; break;  // GND
    case 24: baseX = 885.0;  baseY = 859.5; break;  // VIN
    case 25: baseX = 844.5;  baseY = 859.5; break;  // GND (2)
    case 26: baseX = 559.5;  baseY = 42.0;  break;  // GND (top)

    default:
        qDebug() << "Warning: Unknown pin" << pin;
        return QPoint(boardX + 100, boardY + 100);
    }

    return QPoint(boardX + (int)(baseX * scaleX), boardY + (int)(baseY * scaleY));
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
        int x = (width() - m_arduinoBoardImage.width()) / 2;
        int y = height() - m_arduinoBoardImage.height() - 30;

        painter.drawPixmap(x, y, m_arduinoBoardImage);
    } else {
        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect(), Qt::AlignCenter,
                         "Arduino Board\n(Add arduino_board.png to resources/images/)");
    }
}
