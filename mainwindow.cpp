#include "mainwindow.h"
#include <QFont>
#include <QDebug>
#include <QLabel>
#include "pinselectordialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isCompilationSuccessful(false)
    , compiler(nullptr)
    , simulationEngine(nullptr)
{
    // Create compiler and simulation engine
    compiler = new ArduinoCompiler(this);
    simulationEngine = new SimulationEngine(this);

    // Connect compiler signals
    connect(compiler, &ArduinoCompiler::compilationStarted,
            this, &MainWindow::onCompilationStarted);
    connect(compiler, &ArduinoCompiler::compilationOutput,
            this, &MainWindow::onCompilationOutput);
    connect(compiler, &ArduinoCompiler::compilationError,
            this, &MainWindow::onCompilationError);
    connect(compiler, &ArduinoCompiler::compilationFinished,
            this, &MainWindow::onCompilationFinished);

    // Connect simulation signals
    connect(simulationEngine, &SimulationEngine::pinStateChanged,
            this, &MainWindow::onPinStateChanged);
    connect(simulationEngine, &SimulationEngine::simulationLog,
            this, &MainWindow::onSimulationLog);

    setupUI();
}

MainWindow::~MainWindow()
{
    if (simulationEngine) {
        simulationEngine->stop();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Arduino Simulator - Qt6");
    resize(1200, 800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    createControlPanel(mainLayout);
    createMiddleSection(mainLayout);
    createDebugPanel(mainLayout);

    logMessage("Arduino Simulator initialized. Ready to build.");

    // Check if Arduino CLI is installed
    if (compiler->isArduinoCLIInstalled()) {
        logMessage("✓ Arduino CLI detected and ready");
    } else {
        logMessage("✗ WARNING: Arduino CLI not found. Compilation will fail.");
        logMessage("  Install it from: https://arduino.github.io/arduino-cli/");
    }

    testComponents();
}

void MainWindow::createControlPanel(QVBoxLayout *mainLayout)
{
    QWidget *controlPanel = new QWidget();
    controlPanel->setMaximumHeight(60);
    controlPanel->setStyleSheet("background-color: #e0e0e0; border-radius: 5px;");

    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setContentsMargins(10, 10, 10, 10);

    buildButton = new QPushButton("Build");
    buildButton->setMinimumHeight(40);
    buildButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        );
    connect(buildButton, &QPushButton::clicked, this, &MainWindow::onBuildClicked);

    runButton = new QPushButton("Run");
    runButton->setMinimumHeight(40);
    runButton->setEnabled(false);
    runButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: #0b7dda;"
        "}"
        "QPushButton:pressed:enabled {"
        "   background-color: #0968b8;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #666666;"
        "}"
        );
    connect(runButton, &QPushButton::clicked, this, &MainWindow::onRunClicked);

    controlLayout->addWidget(buildButton);
    controlLayout->addWidget(runButton);
    controlLayout->addStretch();

    mainLayout->addWidget(controlPanel);
}

void MainWindow::createMiddleSection(QVBoxLayout *mainLayout)
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Code editor
    codeEditor = new QTextEdit();
    QFont monoFont("Consolas");
    monoFont.setPointSize(11);
    codeEditor->setFont(monoFont);
    codeEditor->setStyleSheet(
        "QTextEdit {"
        "   background-color: #1e1e1e;"
        "   color: #d4d4d4;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
        );

    // Sample Arduino code
    codeEditor->setPlainText(
        "void setup() {\n"
        "  pinMode(13, OUTPUT);\n"
        "}\n\n"
        "void loop() {\n"
        "  digitalWrite(13, HIGH);\n"
        "  delay(1000);\n"
        "  digitalWrite(13, LOW);\n"
        "  delay(1000);\n"
        "}"
        );

    // Simulator panel
    simulatorPanel = new SimulatorPanel();

    splitter->addWidget(codeEditor);
    splitter->addWidget(simulatorPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>({500, 500}));

    mainLayout->addWidget(splitter, 1);
}

void MainWindow::createDebugPanel(QVBoxLayout *mainLayout)
{
    // Create horizontal splitter for two output panels
    QSplitter *outputSplitter = new QSplitter(Qt::Horizontal);
    outputSplitter->setMaximumHeight(200);

    // ===== STATUS OUTPUT (Left) =====
    QWidget *statusWidget = new QWidget();
    QVBoxLayout *statusLayout = new QVBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(0);

    QLabel *statusLabel = new QLabel("Status Messages");
    statusLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #1e1e1e;"
        "   color: #ffffff;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "   font-size: 11px;"
        "   border: 1px solid #555555;"
        "}"
        );
    statusLayout->addWidget(statusLabel);

    statusOutput = new QTextBrowser();
    statusOutput->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #2b2b2b;"
        "   color: #00ff00;"
        "   font-family: 'Consolas';"
        "   font-size: 10px;"
        "   border: 1px solid #555555;"
        "   border-top: none;"
        "   padding: 5px;"
        "}"
        );
    statusLayout->addWidget(statusOutput);

    // ===== COMPILATION OUTPUT (Right) =====
    QWidget *compilationWidget = new QWidget();
    QVBoxLayout *compilationLayout = new QVBoxLayout(compilationWidget);
    compilationLayout->setContentsMargins(0, 0, 0, 0);
    compilationLayout->setSpacing(0);

    QLabel *compilationLabel = new QLabel("Compilation Output");
    compilationLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #1e1e1e;"
        "   color: #ffffff;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "   font-size: 11px;"
        "   border: 1px solid #555555;"
        "}"
        );
    compilationLayout->addWidget(compilationLabel);

    compilationOutput = new QTextBrowser();
    compilationOutput->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #2b2b2b;"
        "   color: #ffaa00;"
        "   font-family: 'Consolas';"
        "   font-size: 10px;"
        "   border: 1px solid #555555;"
        "   border-top: none;"
        "   padding: 5px;"
        "}"
        );
    compilationLayout->addWidget(compilationOutput);

    // Add both panels to splitter
    outputSplitter->addWidget(statusWidget);
    outputSplitter->addWidget(compilationWidget);
    outputSplitter->setStretchFactor(0, 1);
    outputSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(outputSplitter);
}

void MainWindow::onBuildClicked()
{
    // Clear compilation output
    compilationOutput->clear();

    logMessage(QString("=").repeated(60), true);
    logMessage("BUILD STARTED", true);
    logMessage(QString("=").repeated(60), true);

    QString code = codeEditor->toPlainText();

    if (code.trimmed().isEmpty()) {
        logMessage("ERROR: Code editor is empty!", true);
        isCompilationSuccessful = false;
        runButton->setEnabled(false);
        return;
    }

    // Start compilation using Arduino CLI
    compiler->compile(code);
}

void MainWindow::onRunClicked()
{
    if (!isCompilationSuccessful) {
        logMessage("ERROR: Cannot run. Build the code first.");
        return;
    }

    // Clear status output
    statusOutput->clear();

    logMessage(QString("=").repeated(60));
    logMessage("SIMULATION STARTED");
    logMessage(QString("=").repeated(60));

    QString code = codeEditor->toPlainText();
    simulationEngine->start(code);
}

void MainWindow::onCompilationStarted()
{
    logMessage("Compiling with Arduino CLI...", true);
    buildButton->setEnabled(false);
}

void MainWindow::onCompilationOutput(const QString &output)
{
    compilationOutput->append(output);
    QTextCursor cursor = compilationOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    compilationOutput->setTextCursor(cursor);
}

void MainWindow::onCompilationError(const QString &error)
{
    compilationOutput->setTextColor(QColor(255, 100, 100));
    compilationOutput->append(error);
    compilationOutput->setTextColor(QColor(255, 170, 0));

    QTextCursor cursor = compilationOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    compilationOutput->setTextCursor(cursor);
}

void MainWindow::onCompilationFinished(bool success)
{
    buildButton->setEnabled(true);
    isCompilationSuccessful = success;
    runButton->setEnabled(success);

    if (success) {
        logMessage(QString("=").repeated(60), true);
        logMessage("✓ BUILD SUCCESSFUL - Ready to run simulation", true);
        logMessage(QString("=").repeated(60), true);
    } else {
        logMessage(QString("=").repeated(60), true);
        logMessage("✗ BUILD FAILED - Fix errors and try again", true);
        logMessage(QString("=").repeated(60), true);
    }
}

void MainWindow::onPinStateChanged(int pin, PinState state)
{
    qDebug() << "PIN STATE CHANGED:" << pin << "=" << (state == PIN_HIGH ? "HIGH" : "LOW");

    // Update LED component if it exists for this pin
    if (ledComponents.contains(pin)) {
        ledComponents[pin]->setState(state == PIN_HIGH);
        logMessage(QString("LED Pin %1 turned %2").arg(pin).arg(state == PIN_HIGH ? "ON ✅" : "OFF ❌"));
    } else {
        logMessage(QString("Warning: No LED component for pin %1").arg(pin));
    }
}

void MainWindow::onSimulationLog(const QString &message)
{
    logMessage(message);
}

void MainWindow::logMessage(const QString &message, bool isCompilation)
{
    if (isCompilation) {
        compilationOutput->append(message);
        QTextCursor cursor = compilationOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        compilationOutput->setTextCursor(cursor);
    } else {
        statusOutput->append(message);
        QTextCursor cursor = statusOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        statusOutput->setTextCursor(cursor);
    }
}

void MainWindow::testComponents()
{
    // Create LED on pin 13 - with larger size
    testLED = new LEDComponent(simulatorPanel);
    testLED->setPosition(QPoint(50, 40));
    testLED->setPin(13);
    testLED->show();

    // Map pin 13 to LED component
    ledComponents[13] = testLED;

    // Connect LED leg clicks to handler
    connect(testLED, &LEDComponent::legClicked,
            this, &MainWindow::onLEDLegClicked);

    // Connect LED movement to update wires
    connect(testLED, &Component::componentMoved,
            this, &MainWindow::onComponentMoved);

    // Create button on pin 2
    testButton = new ButtonComponent(simulatorPanel);
    testButton->setPosition(QPoint(220, 40));
    testButton->setPin(2);
    testButton->show();

    // Create resistor
    testResistor = new ResistorComponent(simulatorPanel);
    testResistor->setPosition(QPoint(390, 40));
    testResistor->setResistance(220);
    testResistor->show();

    // Manual button control (for testing)
    connect(testButton, &ButtonComponent::buttonPressed, [this]() {
        testLED->setState(true);
        logMessage("Manual: Button pressed - LED ON");
    });

    connect(testButton, &ButtonComponent::buttonReleased, [this]() {
        testLED->setState(false);
        logMessage("Manual: Button released - LED OFF");
    });

    logMessage("Components created: LED (Pin 13), Button (Pin 2), Resistor (220Ω)");
}

void MainWindow::onLEDLegClicked(LEDComponent *led, bool isPositiveLeg)
{
    qDebug() << "=== onLEDLegClicked called ===";
    qDebug() << "LED:" << led << "IsPositive:" << isPositiveLeg;

    logMessage(QString("LED leg clicked: %1").arg(isPositiveLeg ? "Positive (+)" : "Negative (-)"));

    // Open pin selector dialog
    PinSelectorDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        int selectedPin = dialog.getSelectedPin();

        qDebug() << "Pin selected:" << selectedPin;
        logMessage(QString("Selected Arduino Pin: %1").arg(selectedPin));

        // Create wire connection
        createWireConnection(led, isPositiveLeg, selectedPin);

        logMessage(QString("Wire connected from LED %1 leg to Pin %2")
                       .arg(isPositiveLeg ? "positive" : "negative")
                       .arg(selectedPin));
    } else {
        qDebug() << "Pin selection canceled";
        logMessage("Pin selection canceled");
    }

    qDebug() << "=== onLEDLegClicked finished ===";
}

void MainWindow::createWireConnection(LEDComponent *led, bool isPositiveLeg, int arduinoPin)
{
    qDebug() << "=== createWireConnection called ===";
    qDebug() << "LED:" << led;
    qDebug() << "IsPositive:" << isPositiveLeg;
    qDebug() << "Arduino Pin:" << arduinoPin;

    // Create new wire widget
    Wire *wire = new Wire(simulatorPanel);
    qDebug() << "Wire created:" << wire;

    // Get LED leg position
    QPoint ledLegPos = isPositiveLeg ? led->getPositiveLegPos() : led->getNegativeLegPos();
    qDebug() << "LED leg position:" << ledLegPos;

    // Get Arduino pin position
    QPoint arduinoPinPos = simulatorPanel->getPinPosition(arduinoPin);
    qDebug() << "Arduino pin position:" << arduinoPinPos;

    // Set wire endpoints
    wire->setStartPoint(ledLegPos);
    wire->setEndPoint(arduinoPinPos);

    // Set wire color based on leg type
    if (isPositiveLeg) {
        wire->setColor(Qt::red);
        qDebug() << "Wire color: RED (positive)";
    } else {
        wire->setColor(Qt::white);
        qDebug() << "Wire color: WHITE (negative)";
    }

    wire->setThickness(3);
    wire->show();
    wire->raise();

    qDebug() << "Wire shown and raised";

    // Store wire connection
    WireConnection connection;
    connection.wire = wire;
    connection.led = led;
    connection.arduinoPin = arduinoPin;
    connection.isPositiveLeg = isPositiveLeg;
    wireConnections.append(connection);

    qDebug() << "Wire stored. Total wires:" << wireConnections.size();
    qDebug() << "=== createWireConnection finished ===";
}

void MainWindow::onComponentMoved(Component *component)
{
    LEDComponent *led = qobject_cast<LEDComponent*>(component);
    if (!led) return;

    qDebug() << "Component moved, updating wires...";

    for (WireConnection &conn : wireConnections) {
        if (conn.led == led) {
            QPoint newLegPos = conn.isPositiveLeg ?
                                   led->getPositiveLegPos() :
                                   led->getNegativeLegPos();

            conn.wire->setStartPoint(newLegPos);
            qDebug() << "Wire updated to new position:" << newLegPos;
        }
    }
}
