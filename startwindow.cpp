#include "startwindow.h"
#include "ui_startwindow.h"
#include <QThread>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "simulatorscene.h"
#include "arduinoitem.h"
#include "leditem.h"
#include "cableitem.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartWindow)
    , new_element(nullptr)
    , m_scene(nullptr)
    , m_diagram(nullptr)
{
    ui->setupUi(this);

    // Create scene and diagram
    m_scene = new SimulatorScene(this);
    ui->graphicsView->setScene(m_scene);
    
    // Create diagram - it will create Arduino board internally
    m_diagram = new Diagram(m_scene, this);
    
    // Store diagram reference in scene so pins can access it
    // We'll use a different approach - store it as user data
    m_scene->setProperty("DiagramPointer", QVariant::fromValue<Diagram*>(m_diagram));

    // Connect PushButtons
    connect(ui->pushButton_add_new_control_element, &QPushButton::clicked, this, [this]() {
        if (!new_element) {
            new_element = new Add_New_Element(this);
            // Connect signals from Add_New_Element to our slots
            connect(new_element, &Add_New_Element::ledRequested, this, &StartWindow::onLedRequested);
            connect(new_element, &Add_New_Element::resistorRequested, this, &StartWindow::onResistorRequested);
            connect(new_element, &Add_New_Element::buttonRequested, this, &StartWindow::onButtonRequested);
            
            // Delete the window when it's closed and reset pointer to nullptr
            connect(new_element, &QObject::destroyed, this, [this]() {
                new_element = nullptr;
            });
            new_element->setAttribute(Qt::WA_DeleteOnClose);
        }
        new_element->show();
        new_element->raise();  // Bring to front
        new_element->activateWindow();  // Activate the window
    });

    // Test LED (using diagram)
    m_diagram->addLED(QPointF(20, 20));

    // Test LED2 (using diagram)
    m_diagram->addLED(QPointF(30, 30));

    // Test flashing LED
    ArduinoItem* arduino = m_diagram->getArduino();
    if (arduino) {
        PinItem* pin7 = arduino->getPin(7);
        QTimer* timer = new QTimer(this);
        bool state = false; // LOW

        connect(timer, &QTimer::timeout, this, [=,&state]() mutable {
            if (state)
                pin7->setLow();
            else
                pin7->setHigh();

            state = !state;
        });

        timer->start(500);
    }
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
    int componentId = m_diagram->addLED(position);
    if (componentId != -1) {
        qDebug() << "LED added with ID:" << componentId << "at position:" << position;
    }
}

void StartWindow::onResistorRequested()
{
    qDebug() << "onResistorRequested called";
    QPointF position = getRandomPosition();
    int componentId = m_diagram->addResistor(position, 220);
    if (componentId != -1) {
        qDebug() << "Resistor added with ID:" << componentId << "at position:" << position;
    }
}

void StartWindow::onButtonRequested()
{
    qDebug() << "onButtonRequested called";
    QPointF position = getRandomPosition();
    int componentId = m_diagram->addButton(position);
    if (componentId != -1) {
        qDebug() << "Button added with ID:" << componentId << "at position:" << position;
    }
}


