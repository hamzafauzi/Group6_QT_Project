#ifndef ARDUINORUNTIME_H
#define ARDUINORUNTIME_H
#include <QObject>
#include "arduinoitem.h"
#include <QTimer>

class ArduinoRuntime : public QObject
{
    Q_OBJECT
signals:
    void debugOutput(const QString& html);
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
        bool value;// high or low for digitalWrite
        int delayMs;
        //for if and while
        int conditionPin = -1;
        int jumpTarget = -1;
    };

    QList<Command> m_program;
    int m_pc = 0;
    bool m_running = false;
    QTimer m_timer;
    
    //- Added to track the result of the last if statement for proper if/else execution
    bool m_lastIfResult = false;
};

#endif // ARDUINORUNTIME_H