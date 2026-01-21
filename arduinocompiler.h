#ifndef ARDUINOCOMPILER_H
#define ARDUINOCOMPILER_H

#include <QObject>
#include <QProcess>
#include <QString>

class ArduinoCompiler : public QObject
{
    Q_OBJECT

public:
    explicit ArduinoCompiler(QObject *parent = nullptr);
    ~ArduinoCompiler();

    // Compile Arduino code
    void compile(const QString &code);

    // Check if Arduino CLI is installed
    bool isArduinoCLIInstalled();

signals:
    void compilationStarted();
    void compilationOutput(const QString &output);
    void compilationError(const QString &error);
    void compilationFinished(bool success);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    QProcess *m_process;
    QString m_tempDir;
    QString m_sketchPath;

    bool createTempSketch(const QString &code);
    void cleanup();
    QString findArduinoCLI();
};

#endif // ARDUINOCOMPILER_H
