#include <QGraphicsScene>

class SimulatorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit SimulatorScene(QObject* parent = nullptr);

protected:
    // optional: für spätere Erweiterungen (z. B. Kontextmenü)
    void drawBackground(QPainter* painter, const QRectF& rect) override;
};
