#include "startwindow.h"
#include "ui_startwindow.h"
#include <QThread>
#include <QTimer>
#include "simulatorscene.h"
#include "arduinoitem.h"
#include "leditem.h"
#include "cableitem.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    SimulatorScene* scene = new SimulatorScene(this);
    ui->graphicsView->setScene(scene);

    // create Arduino-Board
    ArduinoItem* arduino = new ArduinoItem();
    scene->addItem(arduino);

    // Test LED
    LedItem* led = new LedItem();
    led->setPos(20, 20);
    scene->addItem(led);

    // Test Cable
    CableItem* groundCable = new CableItem(led->groundPin(), arduino->groundPin());
    // Pins must know the cable
    led->groundPin()->addCable(groundCable);
    arduino->groundPin()->addCable(groundCable);
    scene->addItem(groundCable);

    CableItem* signalCable = new CableItem(arduino->getPin(7), led->signalPin());
    led->signalPin()->addCable(signalCable);
    arduino->getPin(7)->addCable(signalCable);
    scene->addItem(signalCable);

    // Test flashing LED
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

StartWindow::~StartWindow()
{
    delete ui;
}


