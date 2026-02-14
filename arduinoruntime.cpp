#include "arduinoruntime.h"
#include <QRegularExpression>

ArduinoRuntime::ArduinoRuntime(ArduinoItem *arduino, QObject *parent)
    : QObject(parent), m_arduino(arduino)
{
    connect(&m_timer, &QTimer::timeout, this, &ArduinoRuntime::executeNext);
}

void ArduinoRuntime::loadCode(const QString &code)
{
    bool codeSupportedBySimulation = true; // if code is not supported we will set this to true and stop the execution in start() method

    m_program.clear();

    emit debugOutput("<span style='color:#4FC3F7;'>=== Parsing Code ===</span><br>");

    QStringList lines = code.split("\n"); // next line to check all line one by one

    for (QString line : lines) // run all the lines one by one and parse them to commands, we will execute them later in start() method
    {
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        // ----------------- allow setup / loop blocks -----------------
        if (line == "void setup() {" || line == "void loop() {" || line == "}")
        {
            emit debugOutput(
                "<span style='color:#90CAF9;'>Ignored: " + line + "</span><br>");

            continue; // do not try to parse as command, but do not error
        }

        bool lineMatched = false; // track if this line matches any supported syntax

        // ----------------- pinMode -----------------
        if (line.contains("pinMode"))
        {
            QRegularExpression rx(R"(pinMode\s*\(\s*(\d+)\s*,\s*(OUTPUT|INPUT)\s*\)\s*;?)");
            auto m = rx.match(line);
            if (m.hasMatch())
            {
                int pin = m.captured(1).toInt();
                bool isOutput = (m.captured(2) == "OUTPUT");

                m_program.append({"pinMode", pin, isOutput, 0}); // reuse value as OUTPUT/INPUT flag

                emit debugOutput(
                    "<span style='color:#FFD54F;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }

        // ----------------- digitalWrite -----------------
        //- Modified condition to exclude if/else/while lines to prevent duplicate parsing
        if (line.contains("digitalWrite") && !line.startsWith("if") && !line.startsWith("else") && !line.startsWith("while"))
        {
            QRegularExpression rx(R"(digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\)\s*;?)");
            QRegularExpressionMatch m = rx.match(line);

            if (m.hasMatch())
            {
                int pin = m.captured(1).toInt();
                bool high = line.contains("HIGH");

                m_program.append({"digitalWrite", pin, high, 0});

                emit debugOutput(
                    "<span style='color:#81C784;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }

        // ----------------- delay -----------------
        if (line.contains("delay"))
        {
            QRegularExpression rx(R"(delay\s*\(\s*(\d+)\s*\)\s*;?)");
            auto m = rx.match(line);
            if (m.hasMatch())
            {
                int ms = m.captured(1).toInt();
                m_program.append({"delay", 0, false, ms});

                emit debugOutput(
                    "<span style='color:#4DD0E1;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }

        // ----------------- if(digitalRead(x))digitalWrite(y, HIGH); -----------------
        if (line.startsWith("if"))
        {
            QRegularExpression rx(R"(if\s*\(\s*digitalRead\s*\(\s*(\d+)\s*\)\s*\)\s*digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\)\s*;?)");
            auto m = rx.match(line);
            if (m.hasMatch())
            {
                int condPin = m.captured(1).toInt();   // condition pin
                int writePin = m.captured(2).toInt();  // target pin
                bool high = (m.captured(3) == "HIGH"); // target value

                // store: name=if, pin=conditionPin, value=HIGH/LOW, delayMs=writePin (reused field)
                m_program.append({"if", condPin, high, writePin});

                emit debugOutput(
                    "<span style='color:#BA68C8;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }

        // ----------------- while(digitalRead(x))digitalWrite(y, HIGH); -----------------
        if (line.startsWith("while"))
        {
            QRegularExpression rx(R"(while\s*\(\s*digitalRead\s*\(\s*(\d+)\s*\)\s*\)\s*digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\)\s*;?)");
            auto m = rx.match(line);
            if (m.hasMatch())
            {
                int condPin = m.captured(1).toInt();   // condition pin
                int writePin = m.captured(2).toInt();  // target pin
                bool high = (m.captured(3) == "HIGH"); // target value

                // store: name=while, pin=conditionPin, value=HIGH/LOW, delayMs=writePin (reused field)
                m_program.append({"while", condPin, high, writePin});

                emit debugOutput(
                    "<span style='color:#CE93D8;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }
        // ----------------- else -----------------
        if (line.startsWith("else"))
        {
            QRegularExpression rx(R"(else\s+digitalWrite\s*\(\s*(\d+)\s*,\s*(HIGH|LOW)\s*\)\s*;?)");
            auto m = rx.match(line);
            if (m.hasMatch())
            {
                int pin = m.captured(1).toInt();
                bool high = (m.captured(2) == "HIGH");

                // name = "else", pin = target pin, value = HIGH/LOW
                m_program.append({"else", pin, high, 0});

                emit debugOutput(
                    "<span style='color:#FFB74D;'>Parsed: " + line + "</span><br>");

                lineMatched = true; // mark as valid
            }
        }

        // ----------------- unsupported line check -----------------
        if (!lineMatched)
        {
            // mark code as not supported
            codeSupportedBySimulation = false;

            // show error to user
            emit debugOutput(
                "<span style='color:#E57373;'>Syntax not supported: " + line + "</span><br>");

            // stop parsing further
            break;
        }
    }

    emit debugOutput("<span style='color:#4FC3F7;'>=== Parsing Finished ===</span><br>");

    // if code is not supported, clear program so start() will not run anything
    if (!codeSupportedBySimulation)
    {
        m_program.clear(); // prevent execution
    }

    m_pc = 0;
}

void ArduinoRuntime::executeNext()
{
    if (!m_running)
        return;

    if (m_pc >= m_program.size())
        m_pc = 0; // loop again

    Command c = m_program[m_pc++];

    if (c.name == "pinMode")
    {
        emit debugOutput(
            "<span style='color:#FFD54F;'>Executing: pinMode(" +
            QString::number(c.pin) + ", " + (c.value ? "OUTPUT" : "INPUT") + ")</span><br>");

        PinItem *pin = m_arduino->getPin(c.pin);

        if (pin)
        {
            // here you can later set pin mode if you add such logic
        }

        executeNext(); // continue immediately
        //- Added return to prevent fall-through to other command handlers
        return;
    }

    if (c.name == "digitalWrite")
    {
        emit debugOutput(
            "<span style='color:#81C784;'>Executing: digitalWrite(" +
            QString::number(c.pin) + ", " + (c.value ? "HIGH" : "LOW") + ")</span><br>");

        PinItem *pin = m_arduino->getPin(c.pin);

        if (pin)
        {
            if (c.value)
                pin->setHigh();
            else
                pin->setLow();
        }

        // immediately continue
        executeNext();
        //- Added return to prevent fall-through to other command handlers
        return;
    }

    if (c.name == "delay")
    {
        emit debugOutput(
            "<span style='color:#4DD0E1;'>Executing: delay(" +
            QString::number(c.delayMs) + ")</span><br>");

        m_timer.start(c.delayMs);
        //- Added return to prevent fall-through to other command handlers
        return;
    }

    if (c.name == "if")
    {
        // pin = condition pin
        PinItem *condPin = m_arduino->getPin(c.pin);

        bool state = condPin && condPin->isHigh(); // read pin state
        
        //- Store the if result for the else statement to use
        m_lastIfResult = state;

        //- Enhanced debug output to show the condition result
        emit debugOutput(
            "<span style='color:#BA68C8;'>Executing: if(digitalRead(" +
            QString::number(c.pin) + ")) = " + (state ? "TRUE" : "FALSE") + "</span><br>");

        if (state)
        {
            // delayMs is reused as target pin
            PinItem *targetPin = m_arduino->getPin(c.delayMs);
            if (targetPin)
            {
                if (c.value)
                    targetPin->setHigh();
                else
                    targetPin->setLow();
                    
                //- Added debug output to show what digitalWrite was executed
                emit debugOutput(
                    "<span style='color:#BA68C8;'>  → digitalWrite(" +
                    QString::number(c.delayMs) + ", " + (c.value ? "HIGH" : "LOW") + ")</span><br>");
            }
        }

        executeNext(); // continue
        //- Added return to prevent fall-through to other command handlers
        return;
    }
    if (c.name == "else")
    {
        //- Modified to only execute if the previous if was false
        if (!m_lastIfResult)
        {
            emit debugOutput(
                "<span style='color:#FFB74D;'>Executing: else digitalWrite(" +
                QString::number(c.pin) + ", " + (c.value ? "HIGH" : "LOW") + ")</span><br>");

            PinItem *pin = m_arduino->getPin(c.pin);

            if (pin)
            {
                if (c.value)
                    pin->setHigh();
                else
                    pin->setLow();
            }
        }
        //- Added else clause to show when else is skipped
        else
        {
            emit debugOutput(
                "<span style='color:#FFB74D;'>Skipping: else (if was true)</span><br>");
        }

        executeNext(); // continue
        //- Added return to prevent fall-through to other command handlers
        return;
    }

    if (c.name == "while")
    {
        // pin = condition pin
        PinItem *condPin = m_arduino->getPin(c.pin);

        bool state = condPin && condPin->isHigh(); // read pin state

        //- Enhanced debug output to show the condition result
        emit debugOutput(
            "<span style='color:#CE93D8;'>Executing: while(digitalRead(" +
            QString::number(c.pin) + ")) = " + (state ? "TRUE" : "FALSE") + "</span><br>");

        if (state)
        {
            // delayMs is reused as target pin
            PinItem *targetPin = m_arduino->getPin(c.delayMs);
            if (targetPin)
            {
                if (c.value)
                    targetPin->setHigh();
                else
                    targetPin->setLow();
            }

            // go back one instruction to repeat while
            m_pc--; // stay on this while command
        }

        executeNext(); // continue
        //- Added return to prevent fall-through to other command handlers
        return;
    }
}

void ArduinoRuntime::start()
{
    if (m_program.isEmpty())
        return;

    emit debugOutput("<span style='color:#4FC3F7;'> vvv Program Started vvv </span><br>");

    m_running = true;
    m_pc = 0;
    //- Initialize m_lastIfResult when program starts
    m_lastIfResult = false;
    executeNext();
}

void ArduinoRuntime::stop()
{
    emit debugOutput("<span style='color:#E57373;'> ^^^ Program Stopped ^^^ </span><br>");

    m_running = false;
    m_timer.stop();
    m_pc = 0;
    //- Reset m_lastIfResult when program stops
    m_lastIfResult = false;
}