#include "arduinoruntime.h"
#include <QRegularExpression>

ArduinoRuntime::ArduinoRuntime(ArduinoItem* arduino, QObject* parent)
    : QObject(parent), m_arduino(arduino)
{
    connect(&m_timer, &QTimer::timeout, this, &ArduinoRuntime::executeNext);
}

void ArduinoRuntime::loadCode(const QString& code)
{
    m_program.clear();

    emit debugOutput("<span style='color:#4FC3F7;'>=== Parsing Code ===</span><br>");

    QStringList lines = code.split("\n");

    for (QString line : lines)
    {
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        // digitalWrite
        if (line.contains("digitalWrite"))
        {
            QRegularExpression rx(R"(digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\))");
            QRegularExpressionMatch m = rx.match(line);

            if (m.hasMatch()) {
                int pin = m.captured(1).toInt();
                bool high = line.contains("HIGH");

                m_program.append({"digitalWrite", pin, high, 0});

                emit debugOutput(
                    "<span style='color:#81C784;'>Parsed: " + line + "</span><br>");
            }
        }

        // delay
        if (line.contains("delay"))
        {
            QRegularExpression rx(R"(delay\s*\(\s*(\d+)\s*\))");
            auto m = rx.match(line);
            if (m.hasMatch()) {
                int ms = m.captured(1).toInt();
                m_program.append({"delay", 0, false, ms});

                emit debugOutput(
                    "<span style='color:#4DD0E1;'>Parsed: " + line + "</span><br>");
            }
        }

        // if (digitalRead(...))  → şimdilik sadece logluyoruz
        if (line.startsWith("if"))
        {
            emit debugOutput(
                "<span style='color:#BA68C8;'>Parsed IF: " + line + "</span><br>");
            // Şimdilik execution yok, sadece debug
        }
    }

    emit debugOutput("<span style='color:#4FC3F7;'>=== Parsing Finished ===</span><br>");

    m_pc = 0;
}

void ArduinoRuntime::executeNext()
{
    if (!m_running)
        return;

    if (m_pc >= m_program.size())
        m_pc = 0; // loop again

    Command c = m_program[m_pc++];

    if (c.name == "digitalWrite")
    {
        emit debugOutput(
            "<span style='color:#81C784;'>Executing: digitalWrite(" +
            QString::number(c.pin) + ", " + (c.value ? "HIGH" : "LOW") + ")</span><br>");

        PinItem* pin = m_arduino->getPin(c.pin);

        if (pin)
        {
            if (c.value)
                pin->setHigh();
            else
                pin->setLow();
        }

        // immediately continue
        executeNext();
    }

    if (c.name == "delay")
    {
        emit debugOutput(
            "<span style='color:#4DD0E1;'>Executing: delay(" +
            QString::number(c.delayMs) + ")</span><br>");

        m_timer.start(c.delayMs);
    }
}

void ArduinoRuntime::start()
{
    if (m_program.isEmpty())
        return;

    emit debugOutput("<span style='color:#4FC3F7;'> vvv Program Started vvv </span><br>");

    m_running = true;
    m_pc = 0;
    executeNext();
}

void ArduinoRuntime::stop()
{
    emit debugOutput("<span style='color:#E57373;'> ^^^ Program Stopped ^^^ </span><br>");

    m_running = false;
    m_timer.stop();
    m_pc = 0;
}
