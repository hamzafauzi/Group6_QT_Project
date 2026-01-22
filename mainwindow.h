#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMap>
#include "ledcomponent.h"
#include "buttoncomponent.h"
#include "resistorcomponent.h"
#include "simulatorpanel.h"
#include "arduinocompiler.h"
#include "simulationengine.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBuildClicked();
    void onRunClicked();

    // Compiler slots
    void onCompilationStarted();
    void onCompilationOutput(const QString &output);
    void onCompilationError(const QString &error);
    void onCompilationFinished(bool success);

    // Simulation slots
    void onPinStateChanged(int pin, PinState state);
    void onSimulationLog(const QString &message);

private:
    // UI Components
    QPushButton *buildButton;
    QPushButton *runButton;
    QTextEdit *codeEditor;
    SimulatorPanel *simulatorPanel;
    QTextBrowser *statusOutput;        // Status messages panel
    QTextBrowser *compilationOutput;   // Compilation output panel

    // Test components
    LEDComponent *testLED;
    ButtonComponent *testButton;
    ResistorComponent *testResistor;

    // Arduino components
    ArduinoCompiler *compiler;
    SimulationEngine *simulationEngine;

    // Component mapping (pin -> component)
    QMap<int, LEDComponent*> ledComponents;

    // Helper methods
    void setupUI();
    void createControlPanel(QVBoxLayout *mainLayout);
    void createMiddleSection(QVBoxLayout *mainLayout);
    void createDebugPanel(QVBoxLayout *mainLayout);
    void logMessage(const QString &message, bool isCompilation = false);
    void testComponents();

    // Compilation flag
    bool isCompilationSuccessful;
};

#endif // MAINWINDOW_H
