#include "simulationengine.h"
#include <QDebug>
#include <QRegularExpression>

SimulationEngine::SimulationEngine(QObject *parent)
    : QObject(parent)
    , m_loopTimer(new QTimer(this))
    , m_isRunning(false)
    , m_loopCount(0)
{
    connect(m_loopTimer, &QTimer::timeout, this, &SimulationEngine::executeLoop);
}

SimulationEngine::~SimulationEngine()
{
    stop();
}

void SimulationEngine::start(const QString &code)
{
    if (m_isRunning) {
        stop();
    }

    emit simulationLog("=== Starting Simulation ===");

    // Parse the code
    if (!m_parser.parseCode(code)) {
        emit simulationLog("ERROR: Failed to parse Arduino code");
        return;
    }

    emit simulationLog("Code parsed successfully");
    emit simulationLog("Setup code: " + m_parser.getSetupCode());
    emit simulationLog("Loop code: " + m_parser.getLoopCode());

    // Execute setup() once
    executeSetup();

    // Start loop() execution
    m_isRunning = true;
    m_loopCount = 0;
    m_loopTimer->start(10); // Run loop every 10ms (will be adjusted by delay())

    emit simulationLog("Simulation started - executing loop()");
}

void SimulationEngine::stop()
{
    if (m_isRunning) {
        m_loopTimer->stop();
        m_isRunning = false;
        emit simulationLog("=== Simulation Stopped ===");
        emit simulationFinished();
    }
}

void SimulationEngine::executeSetup()
{
    emit simulationLog("\n--- Executing setup() ---");

    QString setupCode = m_parser.getSetupCode();
    QStringList statements = m_parser.extractFunctionCalls(setupCode);

    for (const QString &statement : statements) {
        executeStatement(statement);
    }

    emit simulationLog("--- setup() completed ---\n");
}

void SimulationEngine::executeLoop()
{
    m_loopCount++;

    if (m_loopCount % 100 == 1) { // Log every 100th loop to avoid spam
        emit simulationLog(QString("--- Executing loop() [iteration %1] ---").arg(m_loopCount));
    }

    QString loopCode = m_parser.getLoopCode();
    QStringList statements = m_parser.extractFunctionCalls(loopCode);

    int totalDelay = 0;

    for (const QString &statement : statements) {
        executeStatement(statement);

        // Check for delay
        if (statement.contains("delay")) {
            int delayMs = m_parser.parseDelay(statement);
            totalDelay += delayMs;
        }
    }

    // Adjust timer interval based on total delay in loop
    if (totalDelay > 0) {
        m_loopTimer->setInterval(totalDelay);
    }
}

void SimulationEngine::executeStatement(const QString &statement)
{
    if (statement.contains("pinMode")) {
        m_parser.parsePinMode(statement);
    }
    else if (statement.contains("digitalWrite")) {
        // Get pin number and state before executing
        QRegularExpression re("digitalWrite\\s*\\(\\s*(\\d+)\\s*,\\s*(\\w+)\\s*\\)");
        QRegularExpressionMatch match = re.match(statement);

        if (match.hasMatch()) {
            int pin = match.captured(1).toInt();

            // Execute the digitalWrite
            m_parser.parseDigitalWrite(statement);

            // Get new state and emit signal
            PinState newState = m_parser.getPinState(pin);
            emit pinStateChanged(pin, newState);
        }
    }
    else if (statement.contains("digitalRead")) {
        m_parser.parseDigitalRead(statement);
    }
    else if (statement.contains("delay")) {
        // Delay is handled in executeLoop
    }
}

QMap<int, PinInfo> SimulationEngine::getPinStates() const
{
    return m_parser.getAllPins();
}
