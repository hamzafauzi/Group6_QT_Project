#ifndef ARDUINORUNTIME_H
#define ARDUINORUNTIME_H
#include <QObject>
#include "arduinoitem.h"
#include <QTimer>

class ArduinoRuntime : public QObject
{
    Q_OBJECT

public:
    explicit ArduinoRuntime(ArduinoItem* arduino, QObject* parent=nullptr);

    void loadCode(const QString& code);
    void start();
    void stop();

private slots:
    void executeNext();

private:
    ArduinoItem* m_arduino;

    struct Command {
        QString name;
        int pin;
        bool value;
        int delayMs;
    };

    QList<Command> m_program;
    int m_pc = 0;
    bool m_running = false;
    QTimer m_timer;
};

#endif // ARDUINORUNTIME_H
