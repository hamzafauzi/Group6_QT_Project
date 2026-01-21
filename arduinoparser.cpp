#include "arduinoparser.h"
#include <QRegularExpression>
#include <QDebug>

ArduinoParser::ArduinoParser()
{
}

bool ArduinoParser::parseCode(const QString &code)
{
    // Extract setup() function
    m_setupCode = extractFunctionBody(code, "setup");

    // Extract loop() function
    m_loopCode = extractFunctionBody(code, "loop");

    if (m_setupCode.isEmpty() && m_loopCode.isEmpty()) {
        qDebug() << "Warning: No setup() or loop() function found";
        return false;
    }

    // Parse setup code to initialize pins
    QStringList setupStatements = extractFunctionCalls(m_setupCode);
    for (const QString &statement : setupStatements) {
        if (statement.contains("pinMode")) {
            parsePinMode(statement);
        }
    }

    return true;
}

QString ArduinoParser::extractFunctionBody(const QString &code, const QString &functionName)
{
    QString cleaned = cleanCode(code);

    // Pattern: void functionName() { ... }
    QRegularExpression re(QString("void\\s+%1\\s*\\(\\s*\\)\\s*\\{([^}]*)\\}").arg(functionName));
    QRegularExpressionMatch match = re.match(cleaned);

    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }

    return QString();
}

QString ArduinoParser::cleanCode(const QString &code)
{
    QString cleaned = code;

    // Remove single-line comments
    cleaned.remove(QRegularExpression("//[^\n]*"));

    // Remove multi-line comments
    cleaned.remove(QRegularExpression("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption));

    return cleaned;
}

QStringList ArduinoParser::extractFunctionCalls(const QString &code)
{
    QStringList statements;
    QString cleaned = cleanCode(code);

    // Split by semicolon
    QStringList parts = cleaned.split(';', Qt::SkipEmptyParts);

    for (QString part : parts) {
        part = part.trimmed();
        if (!part.isEmpty()) {
            statements.append(part);
        }
    }

    return statements;
}

void ArduinoParser::parsePinMode(const QString &statement)
{
    // Pattern: pinMode(pin, MODE)
    QRegularExpression re("pinMode\\s*\\(\\s*(\\d+)\\s*,\\s*(\\w+)\\s*\\)");
    QRegularExpressionMatch match = re.match(statement);

    if (match.hasMatch()) {
        int pin = match.captured(1).toInt();
        QString mode = match.captured(2);

        if (mode == "OUTPUT") {
            setPinMode(pin, PIN_OUTPUT);
            qDebug() << "Parsed: pinMode(" << pin << ", OUTPUT)";
        } else if (mode == "INPUT") {
            setPinMode(pin, PIN_INPUT);
            qDebug() << "Parsed: pinMode(" << pin << ", INPUT)";
        }
    }
}

void ArduinoParser::parseDigitalWrite(const QString &statement)
{
    // Pattern: digitalWrite(pin, STATE)
    QRegularExpression re("digitalWrite\\s*\\(\\s*(\\d+)\\s*,\\s*(\\w+)\\s*\\)");
    QRegularExpressionMatch match = re.match(statement);

    if (match.hasMatch()) {
        int pin = match.captured(1).toInt();
        QString state = match.captured(2);

        if (state == "HIGH") {
            setPinState(pin, PIN_HIGH);
            qDebug() << "Executed: digitalWrite(" << pin << ", HIGH)";
        } else if (state == "LOW") {
            setPinState(pin, PIN_LOW);
            qDebug() << "Executed: digitalWrite(" << pin << ", LOW)";
        }
    }
}

int ArduinoParser::parseDigitalRead(const QString &statement)
{
    // Pattern: digitalRead(pin)
    QRegularExpression re("digitalRead\\s*\\(\\s*(\\d+)\\s*\\)");
    QRegularExpressionMatch match = re.match(statement);

    if (match.hasMatch()) {
        int pin = match.captured(1).toInt();
        return getPinState(pin) == PIN_HIGH ? 1 : 0;
    }

    return 0;
}

int ArduinoParser::parseDelay(const QString &statement)
{
    // Pattern: delay(milliseconds)
    QRegularExpression re("delay\\s*\\(\\s*(\\d+)\\s*\\)");
    QRegularExpressionMatch match = re.match(statement);

    if (match.hasMatch()) {
        int ms = match.captured(1).toInt();
        qDebug() << "Parsed: delay(" << ms << ")";
        return ms;
    }

    return 0;
}

void ArduinoParser::setPinMode(int pin, PinMode mode)
{
    m_pins[pin].mode = mode;
}

void ArduinoParser::setPinState(int pin, PinState state)
{
    m_pins[pin].state = state;
}

PinMode ArduinoParser::getPinMode(int pin) const
{
    return m_pins.value(pin).mode;
}

PinState ArduinoParser::getPinState(int pin) const
{
    return m_pins.value(pin).state;
}
