#ifndef ARDUINOPARSER_H
#define ARDUINOPARSER_H

#include <QString>
#include <QMap>
#include <QStringList>

enum PinMode {
    PIN_INPUT,
    PIN_OUTPUT,
    PIN_UNDEFINED
};

enum PinState {
    PIN_LOW,
    PIN_HIGH
};

struct PinInfo {
    PinMode mode;
    PinState state;

    PinInfo() : mode(PIN_UNDEFINED), state(PIN_LOW) {}
};

class ArduinoParser
{
public:
    ArduinoParser();

    // Parse the code and extract setup/loop functions
    bool parseCode(const QString &code);

    // Get setup and loop code
    QString getSetupCode() const { return m_setupCode; }
    QString getLoopCode() const { return m_loopCode; }

    // Extract function calls
    QStringList extractFunctionCalls(const QString &code);

    // Parse individual statements
    void parsePinMode(const QString &statement);
    void parseDigitalWrite(const QString &statement);
    int parseDigitalRead(const QString &statement);
    int parseDelay(const QString &statement);

    // Pin state management
    void setPinMode(int pin, PinMode mode);
    void setPinState(int pin, PinState state);
    PinMode getPinMode(int pin) const;
    PinState getPinState(int pin) const;

    QMap<int, PinInfo> getAllPins() const { return m_pins; }

private:
    QString m_setupCode;
    QString m_loopCode;
    QMap<int, PinInfo> m_pins;

    QString extractFunctionBody(const QString &code, const QString &functionName);
    QString cleanCode(const QString &code);
};

#endif // ARDUINOPARSER_H
