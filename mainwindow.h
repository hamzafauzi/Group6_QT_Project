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
#include <QList>
#include <QMenu>
#include "ledcomponent.h"
#include "buttoncomponent.h"
#include "resistorcomponent.h"
#include "simulatorpanel.h"
#include "arduinocompiler.h"
#include "simulationengine.h"
#include "wire.h"
#include "pinselectordialog.h"
#include "connectionselectordialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBuildClicked();
    void onRunClicked();
    void onStopClicked();

    // Compiler slots
    void onCompilationStarted();
    void onCompilationOutput(const QString &output);
    void onCompilationError(const QString &error);
    void onCompilationFinished(bool success);

    // Simulation slots
    void onPinStateChanged(int pin, PinState state);
    void onSimulationLog(const QString &message);

    // Component interaction slots
    void onLEDLegClicked(LEDComponent *led, bool isPositiveLeg);
    void onResistorLeadClicked(ResistorComponent *resistor, bool isLeftLead);
    void onButtonTerminalClicked(ButtonComponent *button, int terminalNumber);
    void onComponentMoved(Component *component);
    void onComponentRightClicked(Component *component);

    // Component management slots
    void onAddLED();
    void onAddButton();
    void onAddResistor();
    void onDeleteComponent(Component *component);

private:
    // UI Components
    QPushButton *buildButton;
    QPushButton *runButton;
    QPushButton *stopButton;
    QPushButton *addComponentButton;
    QTextEdit *codeEditor;
    SimulatorPanel *simulatorPanel;
    QTextBrowser *statusOutput;
    QTextBrowser *compilationOutput;

    // Test components
    LEDComponent *testLED;
    ButtonComponent *testButton;
    ResistorComponent *testResistor;

    // Arduino components
    ArduinoCompiler *compiler;
    SimulationEngine *simulationEngine;

    // Component management
    QList<Component*> allComponents;
    QMap<int, LEDComponent*> ledComponents;

    // Wire connection tracking
    enum ConnectionEndpointType {
        ARDUINO_PIN,
        LED_LEG,
        RESISTOR_LEAD,
        BUTTON_TERMINAL
    };

    struct ConnectionEndpoint {
        ConnectionEndpointType type;
        int arduinoPin;              // For ARDUINO_PIN
        LEDComponent *led;           // For LED_LEG
        ResistorComponent *resistor; // For RESISTOR_LEAD
        ButtonComponent *button;     // For BUTTON_TERMINAL
        bool isPositiveLeg;          // For LED_LEG (true = positive, false = negative)
        bool isLeftLead;             // For RESISTOR_LEAD (true = left, false = right)
        int terminalNumber;          // For BUTTON_TERMINAL (1-4)

        ConnectionEndpoint()
            : type(ARDUINO_PIN), arduinoPin(-1), led(nullptr), resistor(nullptr),
            button(nullptr), isPositiveLeg(false), isLeftLead(false), terminalNumber(0) {}
    };

    struct WireConnection {
        Wire *wire;
        ConnectionEndpoint startPoint;
        ConnectionEndpoint endPoint;
    };
    QList<WireConnection> wireConnections;

    // Helper methods
    void setupUI();
    void createControlPanel(QVBoxLayout *mainLayout);
    void createMiddleSection(QVBoxLayout *mainLayout);
    void createDebugPanel(QVBoxLayout *mainLayout);
    void logMessage(const QString &message, bool isCompilation = false);
    void testComponents();
    void createWireConnection(LEDComponent *led, bool isPositiveLeg, int arduinoPin);
    void createResistorToArduinoWire(ResistorComponent *resistor, bool isLeftLead, int arduinoPin);
    void createResistorToLEDWire(ResistorComponent *resistor, bool isLeftLead, LEDComponent *led, bool isPositiveLeg);
    void createResistorToButtonWire(ResistorComponent *resistor, bool isLeftLead, ButtonComponent *button, int terminalNumber);
    void createButtonToArduinoWire(ButtonComponent *button, int terminalNumber, int arduinoPin);
    void createButtonToLEDWire(ButtonComponent *button, int terminalNumber, LEDComponent *led, bool isPositiveLeg);
    void createButtonToResistorWire(ButtonComponent *button, int terminalNumber, ResistorComponent *resistor, bool isLeftLead);
    void deleteWiresForComponent(Component *component);
    void updateWiresForComponent(Component *component);
    QPoint getConnectionPoint(const ConnectionEndpoint &endpoint);

    QList<LEDComponent*> getAllLEDs() const;
    QList<ResistorComponent*> getAllResistors() const;
    QList<ButtonComponent*> getAllButtons() const;

    // Component ID counter
    int nextComponentId;

    // Compilation flag
    bool isCompilationSuccessful;
};

#endif // MAINWINDOW_H
