#include "arduinoruntime.h"
#include <QRegularExpression>

ArduinoRuntime::ArduinoRuntime(ArduinoItem* arduino, QObject* parent) : QObject(parent), m_arduino(arduino) {
    connect(&m_timer, &QTimer::timeout, this, &ArduinoRuntime::executeNext);
}

void ArduinoRuntime::loadCode(const QString& code)
{
    m_program.clear();

    QStringList lines = code.split("\n");

    for (QString line : lines)
    {
        line = line.trimmed();
        // digitalWrite function
        if (line.contains("digitalWrite"))
        {
            QRegularExpression rx(R"(digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\))");
            QRegularExpressionMatch m = rx.match(line);

            if(m.hasMatch()){
                int pin = m.captured(1).toInt();
                bool high = line.contains("HIGH");
                m_program.append({"digitalWrite", pin, high, 0});
            }
        }
        // delay function
        if (line.contains("delay"))
        {
            QRegularExpression rx(R"(delay\s*\(\s*(\d+)\s*\))");
            auto m = rx.match(line);
            if(m.hasMatch()){
                int ms = m.captured(1).toInt();
                m_program.append({"delay", 0, false, ms});
            }
        }
    }
    m_pc = 0;
}

void ArduinoRuntime::executeNext()
{
    if(!m_running){
        return;
    }

    if (m_pc >= m_program.size())
        m_pc = 0;   // start loop again

    Command c = m_program[m_pc++];

    if (c.name == "digitalWrite")
    {
        PinItem* pin = m_arduino->getPin(c.pin);

        if (pin)
        {
            if (c.value)
                pin->setHigh();
            else
                pin->setLow();
        }
        // next command
        executeNext();
    }

    if (c.name == "delay")
    {
        m_timer.start(c.delayMs);
    }
}

void ArduinoRuntime::start()
{
    if (m_program.isEmpty())
        return;

    m_running = true;
    m_pc = 0;

    executeNext();
}

void ArduinoRuntime::stop()
{
    m_running = false;
    m_timer.stop();
    m_pc = 0;
}
