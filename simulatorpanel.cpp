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
        // Scale to fit widget while maintaining aspect ratio
        m_arduinoBoardImage = m_arduinoBoardImage.scaled(
            550, 450,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
    } else {
        qDebug() << "Failed to load Arduino board image";
    }
}

void SimulatorPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw gradient background
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(232, 244, 248));
    gradient.setColorAt(1, QColor(208, 232, 240));
    painter.fillRect(rect(), gradient);

    // Draw border
    painter.setPen(QPen(QColor(74, 144, 164), 2));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);

    // Draw Arduino board image centered
    if (!m_arduinoBoardImage.isNull()) {
        int x = (width() - m_arduinoBoardImage.width()) / 2;
        int y = (height() - m_arduinoBoardImage.height()) / 2;
        painter.drawPixmap(x, y, m_arduinoBoardImage);
    }
}
