#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

#include <QObject>
#include <QTimer>
#include "arduinoparser.h"

class SimulationEngine : public QObject
{
    Q_OBJECT

public:
    explicit SimulationEngine(QObject *parent = nullptr);
    ~SimulationEngine();

    // Start/Stop simulation
    void start(const QString &code);
    void stop();

    bool isRunning() const { return m_isRunning; }

    // Get current pin states
    QMap<int, PinInfo> getPinStates() const;

signals:
    void pinStateChanged(int pin, PinState state);
    void simulationLog(const QString &message);
    void simulationFinished();

private slots:
    void executeLoop();

private:
    ArduinoParser m_parser;
    QTimer *m_loopTimer;
    bool m_isRunning;
    int m_loopCount;

    void executeSetup();
    void executeStatement(const QString &statement);
};

#endif // SIMULATIONENGINE_H
