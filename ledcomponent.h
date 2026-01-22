#ifndef LEDCOMPONENT_H
#define LEDCOMPONENT_H

#include "component.h"
#include <QPixmap>

class LEDComponent : public Component
{
    Q_OBJECT

public:
    explicit LEDComponent(QWidget *parent = nullptr);
    ~LEDComponent();

    void setState(bool on);
    bool isOn() const { return m_isOn; }

    // Get leg positions in parent coordinates
    QPoint getPositiveLegPos() const;
    QPoint getNegativeLegPos() const;

    // Implement pure virtual function from Component
    QString componentType() const override { return "LED"; }  // ADD THIS LINE

signals:
    void legClicked(LEDComponent *led, bool isPositiveLeg);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_isOn;
    QPixmap m_ledOnImage;
    QPixmap m_ledOffImage;

    void loadImages();
    bool isClickOnLeg(const QPoint &clickPos, bool &isPositive);
};

#endif // LEDCOMPONENT_H
