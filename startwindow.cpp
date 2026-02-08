#include "startwindow.h"
#include "ui_startwindow.h"
#include <QThread>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "simulatorscene.h"
#include "arduinoitem.h"
#include "leditem.h"
#include "buttonitem.h"
#include "resistoritem.h"
#include "arduinoitem.h"
#include "arduinoruntime.h"
#include "arduinoCompiler.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::StartWindow), new_element(nullptr)
{
    ui->setupUi(this); // setup ui elements from the .ui file

    // -----------------Compiler Start-----------------
    m_compiler = new ArduinoCompiler(this); // create compiler
    // As output comes in, append it to the compilation output widget
    connect(m_compiler, &ArduinoCompiler::compilationOutput, this, [this](const QString &output)
            {
                if (ui->textEdit_compilation_output)
                {
                    ui->textEdit_compilation_output->append(output);

                    // Auto-scroll to bottom
                    QTextCursor cursor = ui->textEdit_compilation_output->textCursor();
                    cursor.movePosition(QTextCursor::End);
                    ui->textEdit_compilation_output->setTextCursor(cursor);
                } });
    // as error comes in, also append to compilation output (could be styled differently)
    connect(m_compiler, &ArduinoCompiler::compilationError, this, [=](const QString &text)
            { ui->textEdit_compilation_output->append(text); });
    // clean once the compilation starts
    connect(m_compiler, &ArduinoCompiler::compilationStarted, this, [=]() {});
    // connect build button
    connect(ui->pushButton_build, &QPushButton::clicked, this, [=]()
            {
    QString code = ui->textEdit_editor->toPlainText();
    m_compiler->compile(code); });

    //----------------Compile END----------------

    // Create scene
    m_scene = new SimulatorScene(this);
    ui->graphicsView->setScene(m_scene);

    // Create Arduino-Board
    ArduinoItem *arduino = new ArduinoItem();
    m_scene->addItem(arduino);

    // initial code for editor
    ui->textEdit_editor->setText(
        "void setup() {\n"
        "  pinMode(7, OUTPUT);\n"
        "}\n"
        "\n"
        "void loop() {\n"
        "  digitalWrite(7, HIGH);\n"
        "  delay(500);\n"
        "  digitalWrite(7, LOW);\n"
        "  delay(500);\n"
        "}\n");

    // Store diagram reference in scene so pins can access it
    // We'll use a different approach - store it as user data

    // Initialize compilation output
    if (ui->textEdit_compilation_output)
    {
        ui->textEdit_compilation_output->clear();
        ui->textEdit_compilation_output->append("<span style='color:#4FC3F7; font-weight:bold;'>VVV - ArduinoCLI Compiler! - (Group 6) VVV</span>");
        ui->textEdit_compilation_output->append("<span style='color:#FFFFFF;'>To build our code, click 'Build' button to compile! For more information click info button. </span><br>");
    }

    // Connect PushButtons
    connect(ui->pushButton_add_new_control_element, &QPushButton::clicked, this, [this]()
            {
                if (!new_element)
                {
                    new_element = new Add_New_Element(this);
                    // Connect signals from Add_New_Element to our slots
                    connect(new_element, &Add_New_Element::ledRequested, this, &StartWindow::onLedRequested);
                    connect(new_element, &Add_New_Element::resistorRequested, this, &StartWindow::onResistorRequested);
                    connect(new_element, &Add_New_Element::buttonRequested, this, &StartWindow::onButtonRequested);
                    // Delete the window when it's closed and reset pointer to nullptr
                    connect(new_element, &QObject::destroyed, this, [this]()
                            { new_element = nullptr; });
                    new_element->setAttribute(Qt::WA_DeleteOnClose);
                }
                new_element->show();
                new_element->raise();          // Bring to front
                new_element->activateWindow(); // Activate the window
            });

    ArduinoRuntime *runtime = new ArduinoRuntime(arduino, this);
    connect(ui->pushButton_run, &QPushButton::clicked, this, [=]
            {
        qDebug() << "Run";
        runtime->loadCode(ui->textEdit_editor->toPlainText());
        runtime->start(); });

    connect(ui->pushButton_interrupt, &QPushButton::clicked, this, [=]
            {
        qDebug() << "Interrupt";
        runtime->stop(); });
}

StartWindow::~StartWindow()
{
    delete ui;
}

QPointF StartWindow::getRandomPosition()
{
    // Base position at 30, 30 with random offset
    int randomX = QRandomGenerator::global()->bounded(-50, 51); // -50 to +50
    int randomY = QRandomGenerator::global()->bounded(-50, 51); // -50 to +50
    return QPointF(30 + randomX, 30 + randomY);
}

void StartWindow::onLedRequested()
{
    qDebug() << "onLedRequested called";
    QPointF position = getRandomPosition();
    LedItem *led = new LedItem();
    led->setPos(position);
    m_scene->addItem(led);
    qDebug() << "LED added with at position:" << position;
}

void StartWindow::onResistorRequested()
{
    qDebug() << "onResistorRequested called";
    QPointF position = getRandomPosition();
    ResistorItem *resistor = new ResistorItem();
    resistor->setPos(position);
    m_scene->addItem(resistor);
    qDebug() << "Resistor added at position:" << position;
}

void StartWindow::onButtonRequested()
{
    qDebug() << "onButtonRequested called";
    QPointF position = getRandomPosition();
    ButtonItem *button = new ButtonItem();
    button->setPos(position);
    m_scene->addItem(button);
    qDebug() << "Button added at position:" << position;
}

// Slot to handle connection logging
void StartWindow::onConnectionLogged(const QString &message)
{
    // Log to compilation output widget
    if (ui->textEdit_compilation_output)
    {
        ui->textEdit_compilation_output->append(message);

        // Auto-scroll to bottom
        QTextCursor cursor = ui->textEdit_compilation_output->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit_compilation_output->setTextCursor(cursor);
    }

    // Also show in status bar briefly
    if (statusBar())
    {
        statusBar()->showMessage(message, 3000); // Show for 3 seconds
    }

    // Log to console for debugging
    qDebug() << "CONNECTION:" << message;
}
