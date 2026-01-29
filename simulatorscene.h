#include <QGraphicsScene>

class PinItem;

class SimulatorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit SimulatorScene(QObject* parent = nullptr);
    void pinClicked(PinItem* pin);

private:
    PinItem* m_selectedPin = nullptr;

protected:
    // optional: für spätere Erweiterungen (z. B. Kontextmenü)
    void drawBackground(QPainter* painter, const QRectF& rect) override;
};
