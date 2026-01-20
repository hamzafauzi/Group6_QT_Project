#ifndef BUTTONCOMPONENT_H
#define BUTTONCOMPONENT_H

#include "component.h"

class ButtonComponent : public Component
{
    Q_OBJECT

public:
    explicit ButtonComponent(QWidget *parent = nullptr);
    ~ButtonComponent();

    void setPressed(bool pressed);
    bool isPressed() const { return m_isPressed; }

    QString componentType() const override { return "Button"; }

signals:
    void buttonPressed();
    void buttonReleased();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isPressed;
    QPixmap m_buttonUpImage;
    QPixmap m_buttonDownImage;

    void loadImages();
    void createDefaultImages();
};

#endif // BUTTONCOMPONENT_H
