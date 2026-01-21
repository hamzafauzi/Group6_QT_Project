#ifndef SIMULATORPANEL_H
#define SIMULATORPANEL_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>

class SimulatorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SimulatorPanel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_arduinoBoardImage;
    void loadBackgroundImage();
};

#endif // SIMULATORPANEL_H
