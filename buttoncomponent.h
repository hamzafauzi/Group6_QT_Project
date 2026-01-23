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

    // Get terminal positions in parent coordinates
    QPoint getTerminal1Pos() const;  // Top-left
    QPoint getTerminal2Pos() const;  // Bottom-left
    QPoint getTerminal3Pos() const;  // Top-right
    QPoint getTerminal4Pos() const;  // Bottom-right

signals:
    void buttonPressed();
    void buttonReleased();
    void terminalClicked(ButtonComponent *button, int terminalNumber);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isPressed;
    QPixmap m_buttonUpImage;
    QPixmap m_buttonDownImage;

    void loadImages();
    void createDefaultImages();
    bool isClickOnCircle(const QPoint &clickPos);
    bool isClickOnTerminal(const QPoint &clickPos, int &terminalNumber);
};

#endif // BUTTONCOMPONENT_H
