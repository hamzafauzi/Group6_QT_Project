#include "mainwindow.h"
#include <QFont>
#include <QDebug>

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
        logMessage("  Or run: choco install arduino-cli");
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
    QFont monoFont("Consolas");  // Windows monospace font
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
    debugOutput = new QTextBrowser();
    debugOutput->setMaximumHeight(200);
    debugOutput->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #2b2b2b;"
        "   color: #00ff00;"
        "   font-family: 'Consolas';"
        "   font-size: 10px;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    mainLayout->addWidget(debugOutput);
}

void MainWindow::onBuildClicked()
{
    logMessage("\n" + QString("=").repeated(60));
    logMessage("BUILD STARTED");
    logMessage(QString("=").repeated(60));

    QString code = codeEditor->toPlainText();

    if (code.trimmed().isEmpty()) {
        logMessage("ERROR: Code editor is empty!");
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

    logMessage("\n" + QString("=").repeated(60));
    logMessage("SIMULATION STARTED");
    logMessage(QString("=").repeated(60));

    QString code = codeEditor->toPlainText();
    simulationEngine->start(code);
}

void MainWindow::onCompilationStarted()
{
    logMessage("Compiling with Arduino CLI...");
    buildButton->setEnabled(false);
}

void MainWindow::onCompilationOutput(const QString &output)
{
    logMessage(output);
}

void MainWindow::onCompilationError(const QString &error)
{
    debugOutput->setTextColor(QColor(255, 100, 100));
    logMessage(error);
    debugOutput->setTextColor(QColor(0, 255, 0));
}

void MainWindow::onCompilationFinished(bool success)
{
    buildButton->setEnabled(true);
    isCompilationSuccessful = success;
    runButton->setEnabled(success);

    if (success) {
        logMessage(QString("=").repeated(60));
        logMessage("✓ BUILD SUCCESSFUL - Ready to run simulation");
        logMessage(QString("=").repeated(60));
    } else {
        logMessage(QString("=").repeated(60));
        logMessage("✗ BUILD FAILED - Fix errors and try again");
        logMessage(QString("=").repeated(60));
    }
}

void MainWindow::onPinStateChanged(int pin, PinState state)
{
    // Update LED component if it exists for this pin
    if (ledComponents.contains(pin)) {
        ledComponents[pin]->setState(state == PIN_HIGH);
    }
}

void MainWindow::onSimulationLog(const QString &message)
{
    logMessage(message);
}

void MainWindow::logMessage(const QString &message)
{
    debugOutput->append(message);
    QTextCursor cursor = debugOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    debugOutput->setTextCursor(cursor);
}

void MainWindow::testComponents()
{
    // Create LED on pin 13 - with new larger size
    testLED = new LEDComponent(simulatorPanel);
    testLED->setPosition(QPoint(50, 80));  // Adjusted position
    testLED->setPin(13);
    testLED->show();

    ledComponents[13] = testLED;

    // Button
    testButton = new ButtonComponent(simulatorPanel);
    testButton->setPosition(QPoint(180, 80));  // More spacing
    testButton->setPin(2);
    testButton->show();

    // Resistor
    testResistor = new ResistorComponent(simulatorPanel);
    testResistor->setPosition(QPoint(310, 80));
    testResistor->setResistance(220);
    testResistor->show();

    // Manual button control
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
