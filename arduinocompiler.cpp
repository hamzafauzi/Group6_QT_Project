#include "arduinocompiler.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>

ArduinoCompiler::ArduinoCompiler(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
{
}

ArduinoCompiler::~ArduinoCompiler()
{
    cleanup();
}

QString ArduinoCompiler::findArduinoCLI()
{
    QStringList possiblePaths;

    // 👇 YOUR PATH (added on top, nothing else changed)
    possiblePaths << "C:/Users/ibaha/arduino-cli_1.4.1_Windows_64bit/arduino-cli.exe";// this is safa's path
    // Vincent add your path here just like on top.
    possiblePaths << "C:/arduino-cli/arduino-cli.exe";  // Preferred (no spaces)
    possiblePaths << "C:/Program Files/Arduino CLI/arduino-cli.exe";
    possiblePaths << QDir::homePath() + "C:/Usersibaha\arduino-cli_1.4.1_Windows_64bit";
    possiblePaths << QDir::homePath() + "/AppData/Local/Programs/Arduino CLI/arduino-cli.exe";
    possiblePaths << "arduino-cli.exe";

    for (const QString &path : possiblePaths) {
        QFile file(path);
        if (file.exists()) {
            qDebug() << "Found Arduino CLI at:" << path;
            return path;
        }

        QProcess testProcess;
        testProcess.start(path, QStringList() << "version");
        testProcess.waitForFinished(2000);

        if (testProcess.exitCode() == 0) {
            qDebug() << "Found Arduino CLI in PATH:" << path;
            return path;
        }
    }

    qDebug() << "Arduino CLI not found";
    return QString();
}


bool ArduinoCompiler::isArduinoCLIInstalled()
{
    QString cliPath = findArduinoCLI();
    return !cliPath.isEmpty();
}

bool ArduinoCompiler::createTempSketch(const QString &code)
{
    m_tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/arduino_simulator";
    QDir dir;

    if (!dir.exists(m_tempDir)) {
        dir.mkpath(m_tempDir);
    }

    QString sketchName = "temp_sketch";
    QString sketchDir = m_tempDir + "/" + sketchName;

    if (!dir.exists(sketchDir)) {
        dir.mkpath(sketchDir);
    }

    m_sketchPath = sketchDir + "/" + sketchName + ".ino";
    QFile file(m_sketchPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to create sketch file:" << m_sketchPath;
        return false;
    }

    QTextStream out(&file);
    out << code;
    file.close();

    qDebug() << "Created sketch at:" << m_sketchPath;
    return true;
}

void ArduinoCompiler::compile(const QString &code)
{
    QString arduinoCliPath = findArduinoCLI();

    if (arduinoCliPath.isEmpty()) {
        emit compilationError("ERROR: Arduino CLI not found!");
        emit compilationError("Install from: https://arduino.github.io/arduino-cli/");
        emit compilationFinished(false);
        return;
    }

    if (!createTempSketch(code)) {
        emit compilationError("Failed to create temporary sketch file");
        emit compilationFinished(false);
        return;
    }

    if (m_process) {
        m_process->kill();
        m_process->deleteLater();
    }

    m_process = new QProcess(this);
    m_process->setWorkingDirectory(QDir::tempPath());

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ArduinoCompiler::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &ArduinoCompiler::onProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &ArduinoCompiler::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &ArduinoCompiler::onReadyReadStandardError);

    QStringList arguments;
    arguments << "compile"
              << "--fqbn" << "arduino:avr:uno"
              << QDir::toNativeSeparators(m_sketchPath);

    emit compilationStarted();
    emit compilationOutput("Starting Arduino compilation...");

    QString nativeCliPath = QDir::toNativeSeparators(arduinoCliPath);
    emit compilationOutput("Using: " + nativeCliPath);
    emit compilationOutput("Sketch: " + QDir::toNativeSeparators(m_sketchPath));

    m_process->start(nativeCliPath, arguments);

    if (!m_process->waitForStarted(3000)) {
        emit compilationError("ERROR: Failed to start arduino-cli");
        emit compilationFinished(false);
    }
}

void ArduinoCompiler::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);

    if (success) {
        emit compilationOutput("\n=== Compilation Successful ===");
    } else {
        emit compilationError("\n=== Compilation Failed ===");
    }

    emit compilationFinished(success);
}

void ArduinoCompiler::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;

    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = "ERROR: arduino-cli failed to start.\n"
                   "Try moving arduino-cli.exe to C:\\arduino-cli\\";
        break;
    case QProcess::Crashed:
        errorMsg = "ERROR: arduino-cli crashed.";
        break;
    case QProcess::Timedout:
        errorMsg = "ERROR: Compilation timed out.";
        break;
    default:
        errorMsg = "ERROR: Unknown error.";
        break;
    }

    emit compilationError(errorMsg);
}

void ArduinoCompiler::onReadyReadStandardOutput()
{
    if (m_process) {
        QString output = m_process->readAllStandardOutput();
        emit compilationOutput(output);
    }
}

void ArduinoCompiler::onReadyReadStandardError()
{
    if (m_process) {
        QString error = m_process->readAllStandardError();
        emit compilationError(error);
    }
}

void ArduinoCompiler::cleanup()
{
    if (m_process) {
        m_process->kill();
        m_process->deleteLater();
        m_process = nullptr;
    }
}
