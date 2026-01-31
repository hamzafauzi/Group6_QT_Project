#include "buttonitem.h"
#include <QPixmap>

ButtonItem::ButtonItem(QGraphicsItem* parent)
    : ComponentItem(QPixmap(":/resources/images/button_up.png"), parent)
{
    QPixmap pix = pixmap().scaledToWidth(80, Qt::SmoothTransformation);
    setPixmap(pix);
    
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Create 4 pins
    m_pin_top_left = new PinItem(0, PinItem::Digital, this);  // Top-left
    m_pin_top_left->setPos(0, 20);

    m_pin_top_right = new PinItem(0, PinItem::Digital, this);  // Bottom-left
    m_pin_top_right->setPos(70, 20);

    m_pin_bottom_left = new PinItem(0, PinItem::Digital, this);  // Top-right
    m_pin_bottom_left->setPos(0, 40);

    m_pin_bottom_right = new PinItem(0, PinItem::Digital, this);  // Bottom-right
    m_pin_bottom_right->setPos(70, 40);
}

void ButtonItem::updateState()
{
    // Button visual state can be updated here if needed
}

void ButtonItem::onPinChanged(PinItem* changedPin){
    if (m_internal) return;

    m_internal = true;

    // it is not nice so many if-else statements but it is working
    if(!m_pressed){
        if(changedPin == this->pin_top_left()){
            if(changedPin->isHigh()){
                this->pin_top_right()->setHigh();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
            else{
                this->pin_top_right()->setLow();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
        }

        if(changedPin == this->pin_top_right()){
            if(changedPin->isHigh()){
                this->pin_top_left()->setHigh();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
            else{
                this->pin_top_left()->setLow();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
        }

        if(changedPin == this->pin_bottom_left()){
            if(changedPin->isHigh()){
                this->pin_bottom_right()->setHigh();
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
            }
            else{
                this->pin_bottom_right()->setLow();
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
            }
        }

        if(changedPin == this->pin_bottom_right()){
            if(changedPin->isHigh()){
                this->pin_bottom_left()->setHigh();
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
            }
            else{
                this->pin_bottom_left()->setLow();
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
            }
        }
    }
    else{
        if(changedPin == this->pin_top_left()){
            if(changedPin->isHigh()){
                this->pin_top_right()->setHigh();
                this->pin_bottom_left()->setHigh();
                this->pin_bottom_right()->setHigh();
            }
            else{
                this->pin_top_right()->setLow();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
        }

        if(changedPin == this->pin_top_right()){
            if(changedPin->isHigh()){
                this->pin_top_left()->setHigh();
                this->pin_bottom_left()->setHigh();
                this->pin_bottom_right()->setHigh();
            }
            else{
                this->pin_top_left()->setLow();
                this->pin_bottom_left()->setLow();
                this->pin_bottom_right()->setLow();
            }
        }

        if(changedPin == this->pin_bottom_left()){
            if(changedPin->isHigh()){
                this->pin_top_left()->setHigh();
                this->pin_top_right()->setHigh();
                this->pin_bottom_right()->setHigh();
            }
            else{
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
                this->pin_bottom_right()->setLow();
            }
        }

        if(changedPin == this->pin_bottom_right()){
            if(changedPin->isHigh()){
                this->pin_top_left()->setHigh();
                this->pin_top_right()->setHigh();
                this->pin_bottom_left()->setHigh();
            }
            else{
                this->pin_top_left()->setLow();
                this->pin_top_right()->setLow();
                this->pin_bottom_left()->setLow();
            }
        }
    }

    m_internal = false;
}

void ButtonItem::reevaluateAfterRelease(){
    if(this->m_pin_top_left->isHigh()){
        this->m_pin_top_right->setHigh();
    }
    if(this->m_pin_top_right->isHigh()){
        this->m_pin_top_left->setHigh();
    }
    if(this->m_pin_bottom_left->isHigh()){
        this->m_pin_bottom_right->setHigh();
    }
    if(this->m_pin_bottom_right->isHigh()){
        this->m_pin_bottom_left->setHigh();
    }
}

void ButtonItem::syncAll()
{
    PinItem* pins[4] = { m_pin_top_left, m_pin_top_right, m_pin_bottom_left, m_pin_bottom_right };

    // Wenn irgendeiner HIGH → alle HIGH
    for (auto* p : pins)
        if (p->isHigh())
        {
            for (auto* q : pins)
                q->setHigh();
            return;
        }
}

void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_pressed = true;
    QPixmap down(":/resources/images/button_pressed.png");
    setPixmap(down.scaledToWidth(80, Qt::SmoothTransformation));
    syncAll();
    ComponentItem::mousePressEvent(event);
}

void ButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_pressed = false;
    QPixmap down(":/resources/images/button_up.png");
    setPixmap(down.scaledToWidth(80, Qt::SmoothTransformation));
    this->pin_top_left()->setLow();
    this->pin_top_right()->setLow();
    this->pin_bottom_left()->setLow();
    this->pin_bottom_right()->setLow();
    reevaluateAfterRelease();
    ComponentItem::mouseReleaseEvent(event);
}
