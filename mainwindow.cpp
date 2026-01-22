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
    , nextComponentId(0)
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
    controlPanel->setMaximumHeight(70);
    controlPanel->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #f5f5f5, stop:1 #e8e8e8);"
        "   border: 1px solid #c0c0c0;"
        "   border-radius: 8px;"
        "}"
        );

    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 12, 15, 12);
    controlLayout->setSpacing(12);

    // Build Button
    buildButton = new QPushButton("🔨 Build");
    buildButton->setMinimumSize(110, 45);
    buildButton->setCursor(Qt::PointingHandCursor);
    buildButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #66BB6A, stop:1 #4CAF50);"
        "   color: white;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 20px;"
        "   box-shadow: 0 2px 4px rgba(0,0,0,0.2);"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #5CB860, stop:1 #43A047);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #4CAF50, stop:1 #388E3C);"
        "   padding-top: 10px;"
        "   padding-bottom: 6px;"
        "}"
        );
    connect(buildButton, &QPushButton::clicked, this, &MainWindow::onBuildClicked);

    // Run Button
    runButton = new QPushButton("▶️ Run");
    runButton->setMinimumSize(110, 45);
    runButton->setEnabled(false);
    runButton->setCursor(Qt::PointingHandCursor);
    runButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #42A5F5, stop:1 #2196F3);"
        "   color: white;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 20px;"
        "   box-shadow: 0 2px 4px rgba(0,0,0,0.2);"
        "}"
        "QPushButton:hover:enabled {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #1E88E5, stop:1 #1976D2);"
        "}"
        "QPushButton:pressed:enabled {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #1976D2, stop:1 #1565C0);"
        "   padding-top: 10px;"
        "   padding-bottom: 6px;"
        "}"
        "QPushButton:disabled {"
        "   background: #d0d0d0;"
        "   color: #888888;"
        "   box-shadow: none;"
        "}"
        );
    connect(runButton, &QPushButton::clicked, this, &MainWindow::onRunClicked);

    // Separator line
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("QFrame { color: #b0b0b0; }");

    // Add Component Button with dropdown menu
    addComponentButton = new QPushButton("➕ Add Component");
    addComponentButton->setMinimumSize(150, 45);
    addComponentButton->setCursor(Qt::PointingHandCursor);
    addComponentButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #FFA726, stop:1 #FF9800);"
        "   color: white;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 20px;"
        "   box-shadow: 0 2px 4px rgba(0,0,0,0.2);"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #FB8C00, stop:1 #F57C00);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #F57C00, stop:1 #EF6C00);"
        "   padding-top: 10px;"
        "   padding-bottom: 6px;"
        "}"
        "QPushButton::menu-indicator {"
        "   image: none;"
        "   width: 0px;"
        "}"
        );

    // Create styled dropdown menu for component selection
    QMenu *componentMenu = new QMenu(this);
    componentMenu->setStyleSheet(
        "QMenu {"
        "   background-color: white;"
        "   border: 2px solid #FF9800;"
        "   border-radius: 6px;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   padding: 8px 30px 8px 20px;"
        "   font-size: 12px;"
        "   color: #333;"
        "   border-radius: 4px;"
        "   margin: 2px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #FFE0B2;"
        "   color: #E65100;"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background: #e0e0e0;"
        "   margin: 5px 10px;"
        "}"
        );

    // Add menu actions with icons/emojis
    QAction *ledAction = componentMenu->addAction("💡 LED Component");
    QAction *buttonAction = componentMenu->addAction("🔘 Button Component");
    componentMenu->addSeparator();
    QAction *resistorAction = componentMenu->addAction("⚡ Resistor Component");

    connect(ledAction, &QAction::triggered, this, &MainWindow::onAddLED);
    connect(buttonAction, &QAction::triggered, this, &MainWindow::onAddButton);
    connect(resistorAction, &QAction::triggered, this, &MainWindow::onAddResistor);

    addComponentButton->setMenu(componentMenu);

    // Add widgets to layout
    controlLayout->addWidget(buildButton);
    controlLayout->addWidget(runButton);
    controlLayout->addWidget(separator);
    controlLayout->addWidget(addComponentButton);
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
    allComponents.append(testLED);

    // Connect LED leg clicks to handler
    connect(testLED, &LEDComponent::legClicked,
            this, &MainWindow::onLEDLegClicked);

    // Connect LED movement to update wires
    connect(testLED, &Component::componentMoved,
            this, &MainWindow::onComponentMoved);

    // Connect right-click
    connect(testLED, &Component::componentRightClicked,
            this, &MainWindow::onComponentRightClicked);

    // Create button on pin 2
    testButton = new ButtonComponent(simulatorPanel);
    testButton->setPosition(QPoint(220, 40));
    testButton->setPin(2);
    testButton->show();

    allComponents.append(testButton);

    connect(testButton, &Component::componentMoved,
            this, &MainWindow::onComponentMoved);
    connect(testButton, &Component::componentRightClicked,
            this, &MainWindow::onComponentRightClicked);

    // Create resistor
    testResistor = new ResistorComponent(simulatorPanel);
    testResistor->setPosition(QPoint(390, 40));
    testResistor->setResistance(220);
    testResistor->show();

    allComponents.append(testResistor);

    connect(testResistor, &ResistorComponent::leadClicked,
            this, &MainWindow::onResistorLeadClicked);
    connect(testResistor, &Component::componentMoved,
            this, &MainWindow::onComponentMoved);
    connect(testResistor, &Component::componentRightClicked,
            this, &MainWindow::onComponentRightClicked);

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

void MainWindow::onResistorLeadClicked(ResistorComponent *resistor, bool isLeftLead)
{
    qDebug() << "=== onResistorLeadClicked called ===";
    qDebug() << "Resistor:" << resistor << "IsLeftLead:" << isLeftLead;

    logMessage(QString("Resistor lead clicked: %1").arg(isLeftLead ? "Left" : "Right"));

    // Open connection selector dialog
    ConnectionSelectorDialog dialog(this);

    // Populate with available LEDs
    dialog.populateLEDList(getAllLEDs());

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getConnectionType() == ConnectionSelectorDialog::ARDUINO_PIN) {
            int selectedPin = dialog.getSelectedPin();
            qDebug() << "Pin selected:" << selectedPin;
            logMessage(QString("Selected Arduino Pin: %1").arg(selectedPin));

            // Create wire connection for resistor to Arduino pin
            createResistorToArduinoWire(resistor, isLeftLead, selectedPin);

            logMessage(QString("✓ Wire connected from Resistor %1 lead to Pin %2")
                           .arg(isLeftLead ? "left" : "right")
                           .arg(selectedPin));
        } else {
            // LED leg selected
            int ledId = dialog.getSelectedLEDId();
            bool isPositive = dialog.isPositiveLeg();

            // Find the LED component
            LEDComponent *targetLED = nullptr;
            for (auto led : ledComponents) {
                if (led->componentId() == ledId) {
                    targetLED = led;
                    break;
                }
            }

            if (targetLED) {
                // Create wire connection for resistor to LED leg
                createResistorToLEDWire(resistor, isLeftLead, targetLED, isPositive);

                logMessage(QString("✓ Wire connected from Resistor %1 lead to LED (Pin %2) %3 leg")
                               .arg(isLeftLead ? "left" : "right")
                               .arg(targetLED->getPin())
                               .arg(isPositive ? "positive" : "negative"));
            } else {
                logMessage("ERROR: Selected LED not found!");
            }
        }
    } else {
        qDebug() << "Connection selection canceled";
        logMessage("Connection selection canceled");
    }

    qDebug() << "=== onResistorLeadClicked finished ===";
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

    // Store wire connection with new structure
    WireConnection connection;
    connection.wire = wire;

    connection.startPoint.type = LED_LEG;
    connection.startPoint.led = led;
    connection.startPoint.isPositiveLeg = isPositiveLeg;

    connection.endPoint.type = ARDUINO_PIN;
    connection.endPoint.arduinoPin = arduinoPin;

    wireConnections.append(connection);

    qDebug() << "Wire stored. Total wires:" << wireConnections.size();
    qDebug() << "=== createWireConnection finished ===";
}

void MainWindow::createResistorToArduinoWire(ResistorComponent *resistor, bool isLeftLead, int arduinoPin)
{
    qDebug() << "=== createResistorToArduinoWire called ===";

    // Create new wire widget
    Wire *wire = new Wire(simulatorPanel);

    // Get resistor lead position
    QPoint resistorLeadPos = isLeftLead ? resistor->getLeftLeadPos() : resistor->getRightLeadPos();
    qDebug() << "Resistor lead position:" << resistorLeadPos;

    // Get Arduino pin position
    QPoint arduinoPinPos = simulatorPanel->getPinPosition(arduinoPin);
    qDebug() << "Arduino pin position:" << arduinoPinPos;

    // Set wire endpoints
    wire->setStartPoint(resistorLeadPos);
    wire->setEndPoint(arduinoPinPos);

    // Set wire color (yellow for resistors)
    wire->setColor(Qt::yellow);
    wire->setThickness(3);
    wire->show();
    wire->raise();

    // Store wire connection
    WireConnection connection;
    connection.wire = wire;

    connection.startPoint.type = RESISTOR_LEAD;
    connection.startPoint.resistor = resistor;
    connection.startPoint.isLeftLead = isLeftLead;

    connection.endPoint.type = ARDUINO_PIN;
    connection.endPoint.arduinoPin = arduinoPin;

    wireConnections.append(connection);

    qDebug() << "Resistor wire stored. Total wires:" << wireConnections.size();
}

void MainWindow::createResistorToLEDWire(ResistorComponent *resistor, bool isLeftLead, LEDComponent *led, bool isPositiveLeg)
{
    qDebug() << "=== createResistorToLEDWire called ===";

    // Create new wire widget
    Wire *wire = new Wire(simulatorPanel);

    // Get resistor lead position
    QPoint resistorLeadPos = isLeftLead ? resistor->getLeftLeadPos() : resistor->getRightLeadPos();
    qDebug() << "Resistor lead position:" << resistorLeadPos;

    // Get LED leg position
    QPoint ledLegPos = isPositiveLeg ? led->getPositiveLegPos() : led->getNegativeLegPos();
    qDebug() << "LED leg position:" << ledLegPos;

    // Set wire endpoints
    wire->setStartPoint(resistorLeadPos);
    wire->setEndPoint(ledLegPos);

    // Set wire color (green for resistor-to-LED connections)
    wire->setColor(Qt::green);
    wire->setThickness(3);
    wire->show();
    wire->raise();

    // Store wire connection
    WireConnection connection;
    connection.wire = wire;

    connection.startPoint.type = RESISTOR_LEAD;
    connection.startPoint.resistor = resistor;
    connection.startPoint.isLeftLead = isLeftLead;

    connection.endPoint.type = LED_LEG;
    connection.endPoint.led = led;
    connection.endPoint.isPositiveLeg = isPositiveLeg;

    wireConnections.append(connection);

    qDebug() << "Resistor-to-LED wire stored. Total wires:" << wireConnections.size();
}

QPoint MainWindow::getConnectionPoint(const ConnectionEndpoint &endpoint)
{
    switch (endpoint.type) {
    case ARDUINO_PIN:
        return simulatorPanel->getPinPosition(endpoint.arduinoPin);

    case LED_LEG:
        if (endpoint.led) {
            return endpoint.isPositiveLeg ?
                       endpoint.led->getPositiveLegPos() :
                       endpoint.led->getNegativeLegPos();
        }
        break;

    case RESISTOR_LEAD:
        if (endpoint.resistor) {
            return endpoint.isLeftLead ?
                       endpoint.resistor->getLeftLeadPos() :
                       endpoint.resistor->getRightLeadPos();
        }
        break;
    }

    return QPoint(0, 0);
}

void MainWindow::onComponentMoved(Component *component)
{
    qDebug() << "Component moved, updating wires...";

    updateWiresForComponent(component);
}

void MainWindow::updateWiresForComponent(Component *component)
{
    LEDComponent *led = qobject_cast<LEDComponent*>(component);
    ResistorComponent *resistor = qobject_cast<ResistorComponent*>(component);

    for (WireConnection &conn : wireConnections) {
        bool needsUpdate = false;

        // Check if this wire is connected to the moved component
        if (conn.startPoint.type == LED_LEG && conn.startPoint.led == led) {
            QPoint newPos = getConnectionPoint(conn.startPoint);
            conn.wire->setStartPoint(newPos);
            needsUpdate = true;
        }
        else if (conn.startPoint.type == RESISTOR_LEAD && conn.startPoint.resistor == resistor) {
            QPoint newPos = getConnectionPoint(conn.startPoint);
            conn.wire->setStartPoint(newPos);
            needsUpdate = true;
        }

        if (conn.endPoint.type == LED_LEG && conn.endPoint.led == led) {
            QPoint newPos = getConnectionPoint(conn.endPoint);
            conn.wire->setEndPoint(newPos);
            needsUpdate = true;
        }
        else if (conn.endPoint.type == RESISTOR_LEAD && conn.endPoint.resistor == resistor) {
            QPoint newPos = getConnectionPoint(conn.endPoint);
            conn.wire->setEndPoint(newPos);
            needsUpdate = true;
        }

        if (needsUpdate) {
            qDebug() << "Wire updated for moved component";
        }
    }
}

// ========== COMPONENT MANAGEMENT FUNCTIONS ==========

void MainWindow::onComponentRightClicked(Component* component)
{
    if (!component) return;

    logMessage(QString("Right-clicked on %1 (Pin %2)")
                   .arg(component->componentType())
                   .arg(component->getPin()));

    // Show context menu at cursor position
    QMenu contextMenu(this);

    QAction *deleteAction = contextMenu.addAction("🗑️ Delete Component");

    QAction *selectedAction = contextMenu.exec(QCursor::pos());

    if (selectedAction == deleteAction) {
        onDeleteComponent(component);
    }
}

void MainWindow::onAddLED()
{
    LEDComponent *led = new LEDComponent(simulatorPanel);
    led->setPosition(QPoint(50 + (nextComponentId * 60 % 400), 40 + (nextComponentId / 7) * 180));
    led->setPin(13 + nextComponentId);
    led->setComponentId(nextComponentId++);
    led->show();

    allComponents.append(led);
    ledComponents[led->getPin()] = led;

    connect(led, &LEDComponent::legClicked, this, &MainWindow::onLEDLegClicked);
    connect(led, &Component::componentMoved, this, &MainWindow::onComponentMoved);
    connect(led, &Component::componentRightClicked, this, &MainWindow::onComponentRightClicked);

    logMessage(QString("✓ LED added on Pin %1 (ID: %2)")
                   .arg(led->getPin())
                   .arg(led->componentId()));
}

void MainWindow::onAddButton()
{
    ButtonComponent *button = new ButtonComponent(simulatorPanel);
    button->setPosition(QPoint(50 + (nextComponentId * 60 % 400), 40 + (nextComponentId / 7) * 180));
    button->setPin(2 + nextComponentId);
    button->setComponentId(nextComponentId++);
    button->show();

    allComponents.append(button);

    connect(button, &Component::componentMoved, this, &MainWindow::onComponentMoved);
    connect(button, &Component::componentRightClicked, this, &MainWindow::onComponentRightClicked);

    logMessage(QString("✓ Button added on Pin %1 (ID: %2)")
                   .arg(button->getPin())
                   .arg(button->componentId()));
}

void MainWindow::onAddResistor()
{
    ResistorComponent *resistor = new ResistorComponent(simulatorPanel);
    resistor->setPosition(QPoint(50 + (nextComponentId * 60 % 400), 40 + (nextComponentId / 7) * 180));
    resistor->setResistance(220);
    resistor->setComponentId(nextComponentId++);
    resistor->show();

    allComponents.append(resistor);

    connect(resistor, &ResistorComponent::leadClicked, this, &MainWindow::onResistorLeadClicked);
    connect(resistor, &Component::componentMoved, this, &MainWindow::onComponentMoved);
    connect(resistor, &Component::componentRightClicked, this, &MainWindow::onComponentRightClicked);

    logMessage(QString("✓ Resistor added (%1Ω, ID: %2)")
                   .arg(resistor->resistance())
                   .arg(resistor->componentId()));
}

void MainWindow::onDeleteComponent(Component *component)
{
    if (!component) return;

    QString componentInfo = QString("%1 on Pin %2")
                                .arg(component->componentType())
                                .arg(component->getPin());

    // Delete wires connected to this component
    deleteWiresForComponent(component);

    // Remove from ledComponents map if it's an LED
    LEDComponent *led = qobject_cast<LEDComponent*>(component);
    if (led) {
        ledComponents.remove(component->getPin());
    }

    // Remove from allComponents list
    allComponents.removeOne(component);

    logMessage(QString("🗑️ Deleted: %1").arg(componentInfo));

    // Delete the component
    component->deleteLater();
}

void MainWindow::deleteWiresForComponent(Component *component)
{
    int deletedCount = 0;

    LEDComponent *led = qobject_cast<LEDComponent*>(component);
    ResistorComponent *resistor = qobject_cast<ResistorComponent*>(component);

    for (int i = wireConnections.size() - 1; i >= 0; --i) {
        bool shouldDelete = false;

        // Check if wire is connected to this component
        if (led) {
            if ((wireConnections[i].startPoint.type == LED_LEG && wireConnections[i].startPoint.led == led) ||
                (wireConnections[i].endPoint.type == LED_LEG && wireConnections[i].endPoint.led == led)) {
                shouldDelete = true;
            }
        }
        else if (resistor) {
            if ((wireConnections[i].startPoint.type == RESISTOR_LEAD && wireConnections[i].startPoint.resistor == resistor) ||
                (wireConnections[i].endPoint.type == RESISTOR_LEAD && wireConnections[i].endPoint.resistor == resistor)) {
                shouldDelete = true;
            }
        }

        if (shouldDelete) {
            wireConnections[i].wire->deleteLater();
            wireConnections.removeAt(i);
            deletedCount++;
        }
    }

    if (deletedCount > 0) {
        logMessage(QString("  ↳ Deleted %1 wire(s)").arg(deletedCount));
    }
}

QList<LEDComponent*> MainWindow::getAllLEDs() const
{
    QList<LEDComponent*> leds;

    for (Component *component : allComponents) {
        LEDComponent *led = qobject_cast<LEDComponent*>(component);
        if (led) {
            leds.append(led);
        }
    }

    return leds;
}
