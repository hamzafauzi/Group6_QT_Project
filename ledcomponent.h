#ifndef LEDCOMPONENT_H
#define LEDCOMPONENT_H

#include "component.h"

class LEDComponent : public Component
{
    Q_OBJECT

public:
    explicit LEDComponent(QWidget *parent = nullptr);
    ~LEDComponent();

    void setState(bool on);
    bool state() const { return m_isOn; }

    QString componentType() const override { return "LED"; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_isOn;
    QPixmap m_ledOnImage;
    QPixmap m_ledOffImage;

    void loadImages();
    void createDefaultImages();
};

#endif // LEDCOMPONENT_H
