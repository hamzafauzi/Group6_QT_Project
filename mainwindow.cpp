#include "mainwindow.h"
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isCompilationSuccessful(false)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    // Qt handles cleanup automatically for child widgets
}

void MainWindow::setupUI()
{
    // Set window properties
    setWindowTitle("Arduino Simulator - Qt6");
    resize(1200, 800);

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create main vertical layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Add all sections
    createControlPanel(mainLayout);
    createMiddleSection(mainLayout);
    createDebugPanel(mainLayout);

    // Initialize
    logMessage("Arduino Simulator initialized. Ready to build.");

    testComponents();
}

void MainWindow::createControlPanel(QVBoxLayout *mainLayout)
{
    // Create control panel widget
    QWidget *controlPanel = new QWidget();
    controlPanel->setMaximumHeight(60);
    controlPanel->setStyleSheet("background-color: #e0e0e0; border-radius: 5px;");

    // Create horizontal layout for buttons
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setContentsMargins(10, 10, 10, 10);

    // Create Build button
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

    // Create Run button
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

    // Add buttons to layout
    controlLayout->addWidget(buildButton);
    controlLayout->addWidget(runButton);
    controlLayout->addStretch();

    // Add control panel to main layout
    mainLayout->addWidget(controlPanel);
}

void MainWindow::createMiddleSection(QVBoxLayout *mainLayout)
{
    // Create horizontal splitter for code editor and simulator
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Create code editor
    codeEditor = new QTextEdit();
    QFont monoFont("Menlo");
    monoFont.setPointSize(13);
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

    // Add sample Arduino code
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

    // Create simulator panel
    simulatorPanel = new QWidget();
    simulatorPanel->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                stop:0 #e8f4f8, stop:1 #d0e8f0);"
        "   border: 2px solid #4a90a4;"
        "   border-radius: 8px;"
        "}"
        );
    simulatorPanel->setMinimumWidth(500);

    // Add to splitter
    splitter->addWidget(codeEditor);
    splitter->addWidget(simulatorPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>({500, 500}));

    // Add splitter to main layout
    mainLayout->addWidget(splitter, 1);
}

void MainWindow::createDebugPanel(QVBoxLayout *mainLayout)
{
    // Create debug output panel
    debugOutput = new QTextBrowser();
    debugOutput->setMaximumHeight(200);
    debugOutput->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #2b2b2b;"
        "   color: #00ff00;"
        "   font-family: 'Menlo';"
        "   font-size: 11px;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    // Add to main layout
    mainLayout->addWidget(debugOutput);
}

void MainWindow::onBuildClicked()
{
    logMessage("=== Build Started ===");

    // Get the code from editor
    QString code = codeEditor->toPlainText();

    if (code.trimmed().isEmpty()) {
        logMessage("ERROR: Code editor is empty!");
        isCompilationSuccessful = false;
        runButton->setEnabled(false);
        return;
    }

    logMessage("Analyzing code...");
    logMessage("Code length: " + QString::number(code.length()) + " characters");

    // TODO: In next step, we'll integrate Arduino compiler here
    logMessage("Note: Arduino compiler integration pending.");
    logMessage("=== Build Completed (Simulated) ===");

    // For now, simulate successful compilation
    isCompilationSuccessful = true;
    runButton->setEnabled(true);
    logMessage("Build successful! You can now run the simulation.");
}

void MainWindow::onRunClicked()
{
    if (!isCompilationSuccessful) {
        logMessage("ERROR: Cannot run. Please build the code first.");
        return;
    }

    logMessage("=== Simulation Started ===");
    logMessage("Running Arduino simulation...");

    // TODO: In next step, we'll implement simulation logic
    logMessage("Simulation functionality will be implemented in next steps.");
}

void MainWindow::logMessage(const QString &message)
{
    debugOutput->append(message);

    // Auto-scroll to bottom
    QTextCursor cursor = debugOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    debugOutput->setTextCursor(cursor);
}

void MainWindow::testComponents()
{
    // Create test LED with better positioning
    testLED = new LEDComponent(simulatorPanel);
    testLED->setPosition(QPoint(80, 100));
    testLED->setPin(13);
    testLED->show();

    // Create test button
    testButton = new ButtonComponent(simulatorPanel);
    testButton->setPosition(QPoint(200, 100));
    testButton->setPin(2);
    testButton->show();

    // Create test resistor
    testResistor = new ResistorComponent(simulatorPanel);
    testResistor->setPosition(QPoint(320, 100));
    testResistor->setResistance(220);
    testResistor->show();

    // Connect button to LED
    connect(testButton, &ButtonComponent::buttonPressed, [this]() {
        testLED->setState(true);
        logMessage("Button pressed - LED ON");
    });

    connect(testButton, &ButtonComponent::buttonReleased, [this]() {
        testLED->setState(false);
        logMessage("Button released - LED OFF");
    });

    logMessage("Test components created: LED (Pin 13), Button (Pin 2), and Resistor (220Ω)");
}
