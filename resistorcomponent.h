#ifndef RESISTORCOMPONENT_H
#define RESISTORCOMPONENT_H

#include "component.h"

class ResistorComponent : public Component
{
    Q_OBJECT

public:
    explicit ResistorComponent(QWidget *parent = nullptr);
    ~ResistorComponent();

    void setResistance(int ohms);
    int resistance() const { return m_resistance; }

    QString componentType() const override { return "Resistor"; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_resistance;
    QPixmap m_resistorImage;

    void loadImage();
    void createDefaultImage();
};

#endif // RESISTORCOMPONENT_H
