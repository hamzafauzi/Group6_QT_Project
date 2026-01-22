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

    // Get lead positions in parent coordinates
    QPoint getLeftLeadPos() const;
    QPoint getRightLeadPos() const;

signals:
    void leadClicked(ResistorComponent *resistor, bool isLeftLead);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_resistance;
    QPixmap m_resistorImage;

    void loadImage();
    void createDefaultImage();
    bool isClickOnLead(const QPoint &clickPos, bool &isLeftLead);
};

#endif // RESISTORCOMPONENT_H
