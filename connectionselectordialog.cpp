#include "connectionselectordialog.h"
#include "ledcomponent.h"
#include "resistorcomponent.h"
#include "buttoncomponent.h"

ConnectionSelectorDialog::ConnectionSelectorDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void ConnectionSelectorDialog::setupUI()
{
    setWindowTitle("Select Connection Target");
    setModal(true);
    resize(380, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title label
    QLabel *titleLabel = new QLabel("What do you want to connect to?");
    titleLabel->setStyleSheet("font-size: 13px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Connection type selection
    arduinoPinRadio = new QRadioButton("Arduino Pin");
    ledLegRadio = new QRadioButton("LED Leg");
    resistorLeadRadio = new QRadioButton("Resistor Lead");
    buttonTerminalRadio = new QRadioButton("Button Terminal");
    arduinoPinRadio->setChecked(true);

    QVBoxLayout *radioLayout = new QVBoxLayout();
    radioLayout->addWidget(arduinoPinRadio);
    radioLayout->addWidget(ledLegRadio);
    radioLayout->addWidget(resistorLeadRadio);
    radioLayout->addWidget(buttonTerminalRadio);
    mainLayout->addLayout(radioLayout);

    // Arduino Pin selection (shown by default)
    QLabel *pinLabel = new QLabel("Select Arduino Pin:");
    pinLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(pinLabel);

    pinComboBox = new QComboBox();
    pinComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");

    // Add digital pins (0-13)
    for (int i = 0; i <= 13; i++) {
        pinComboBox->addItem(QString("Digital Pin %1").arg(i), i);
    }
    pinComboBox->insertSeparator(pinComboBox->count());

    // Add analog pins (A0-A5 mapped to 14-19)
    for (int i = 0; i <= 5; i++) {
        pinComboBox->addItem(QString("Analog Pin A%1").arg(i), 14 + i);
    }
    pinComboBox->insertSeparator(pinComboBox->count());

    // Add power pins
    pinComboBox->addItem("RESET", 20);
    pinComboBox->addItem("3.3V", 21);
    pinComboBox->addItem("5V", 22);
    pinComboBox->addItem("GND", 23);
    pinComboBox->addItem("VIN", 24);

    mainLayout->addWidget(pinComboBox);

    // LED selection
    QLabel *ledLabel = new QLabel("Select LED Component:");
    ledLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(ledLabel);

    ledComboBox = new QComboBox();
    ledComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    ledComboBox->setEnabled(false);
    mainLayout->addWidget(ledComboBox);

    // Leg type selection
    QLabel *legLabel = new QLabel("Select Leg Type:");
    legLabel->setStyleSheet("font-size: 11px; margin-top: 5px;");
    mainLayout->addWidget(legLabel);

    legTypeComboBox = new QComboBox();
    legTypeComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    legTypeComboBox->addItem("Long Leg (Positive +)", true);
    legTypeComboBox->addItem("Short Leg (Negative -)", false);
    legTypeComboBox->setEnabled(false);
    mainLayout->addWidget(legTypeComboBox);

    // Resistor selection
    QLabel *resistorLabel = new QLabel("Select Resistor Component:");
    resistorLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(resistorLabel);

    resistorComboBox = new QComboBox();
    resistorComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    resistorComboBox->setEnabled(false);
    mainLayout->addWidget(resistorComboBox);

    // Lead type selection
    QLabel *leadLabel = new QLabel("Select Lead:");
    leadLabel->setStyleSheet("font-size: 11px; margin-top: 5px;");
    mainLayout->addWidget(leadLabel);

    leadTypeComboBox = new QComboBox();
    leadTypeComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    leadTypeComboBox->addItem("Left Lead", true);
    leadTypeComboBox->addItem("Right Lead", false);
    leadTypeComboBox->setEnabled(false);
    mainLayout->addWidget(leadTypeComboBox);

    // Button selection
    QLabel *buttonLabel = new QLabel("Select Button Component:");
    buttonLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(buttonLabel);

    buttonComboBox = new QComboBox();
    buttonComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    buttonComboBox->setEnabled(false);
    mainLayout->addWidget(buttonComboBox);

    // Terminal selection
    QLabel *terminalLabel = new QLabel("Select Terminal:");
    terminalLabel->setStyleSheet("font-size: 11px; margin-top: 5px;");
    mainLayout->addWidget(terminalLabel);

    terminalComboBox = new QComboBox();
    terminalComboBox->setStyleSheet("QComboBox { padding: 5px; font-size: 11px; border: 1px solid #ccc; border-radius: 3px; }");
    terminalComboBox->addItem("Terminal 1", 1);
    terminalComboBox->addItem("Terminal 2", 2);
    terminalComboBox->addItem("Terminal 3", 3);
    terminalComboBox->addItem("Terminal 4", 4);
    terminalComboBox->setEnabled(false);
    mainLayout->addWidget(terminalComboBox);

    // Connect radio button signals
    connect(arduinoPinRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);
    connect(ledLegRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);
    connect(resistorLeadRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);
    connect(buttonTerminalRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);

    mainLayout->addSpacing(10);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    okButton = new QPushButton("OK");
    okButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 8px 20px; border: none; border-radius: 4px; font-size: 12px; }"
        "QPushButton:hover { background-color: #45a049; }"
        );
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    cancelButton = new QPushButton("Cancel");
    cancelButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 8px 20px; border: none; border-radius: 4px; font-size: 12px; }"
        "QPushButton:hover { background-color: #da190b; }"
        );
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
}

void ConnectionSelectorDialog::onConnectionTypeChanged()
{
    if (arduinoPinRadio->isChecked()) {
        pinComboBox->setEnabled(true);
        ledComboBox->setEnabled(false);
        legTypeComboBox->setEnabled(false);
        resistorComboBox->setEnabled(false);
        leadTypeComboBox->setEnabled(false);
        buttonComboBox->setEnabled(false);
        terminalComboBox->setEnabled(false);
    }
    else if (ledLegRadio->isChecked()) {
        pinComboBox->setEnabled(false);
        ledComboBox->setEnabled(true);
        legTypeComboBox->setEnabled(true);
        resistorComboBox->setEnabled(false);
        leadTypeComboBox->setEnabled(false);
        buttonComboBox->setEnabled(false);
        terminalComboBox->setEnabled(false);
    }
    else if (resistorLeadRadio->isChecked()) {
        pinComboBox->setEnabled(false);
        ledComboBox->setEnabled(false);
        legTypeComboBox->setEnabled(false);
        resistorComboBox->setEnabled(true);
        leadTypeComboBox->setEnabled(true);
        buttonComboBox->setEnabled(false);
        terminalComboBox->setEnabled(false);
    }
    else if (buttonTerminalRadio->isChecked()) {
        pinComboBox->setEnabled(false);
        ledComboBox->setEnabled(false);
        legTypeComboBox->setEnabled(false);
        resistorComboBox->setEnabled(false);
        leadTypeComboBox->setEnabled(false);
        buttonComboBox->setEnabled(true);
        terminalComboBox->setEnabled(true);
    }
}

ConnectionSelectorDialog::ConnectionType ConnectionSelectorDialog::getConnectionType() const
{
    if (ledLegRadio->isChecked()) return LED_LEG;
    if (resistorLeadRadio->isChecked()) return RESISTOR_LEAD;
    if (buttonTerminalRadio->isChecked()) return BUTTON_TERMINAL;
    return ARDUINO_PIN;
}

int ConnectionSelectorDialog::getSelectedPin() const
{
    return pinComboBox->currentData().toInt();
}

int ConnectionSelectorDialog::getSelectedLEDId() const
{
    return ledComboBox->currentData().toInt();
}

bool ConnectionSelectorDialog::isPositiveLeg() const
{
    return legTypeComboBox->currentData().toBool();
}

int ConnectionSelectorDialog::getSelectedResistorId() const
{
    return resistorComboBox->currentData().toInt();
}

bool ConnectionSelectorDialog::isLeftLead() const
{
    return leadTypeComboBox->currentData().toBool();
}

int ConnectionSelectorDialog::getSelectedButtonId() const
{
    return buttonComboBox->currentData().toInt();
}

int ConnectionSelectorDialog::getSelectedTerminalNumber() const
{
    return terminalComboBox->currentData().toInt();
}

void ConnectionSelectorDialog::populateLEDList(const QList<LEDComponent*> &leds)
{
    ledComboBox->clear();

    if (leds.isEmpty()) {
        ledComboBox->addItem("No LEDs available", -1);
        ledLegRadio->setEnabled(false);
        return;
    }

    int ledNumber = 1;
    for (LEDComponent *led : leds) {
        QString label = QString("LED %1").arg(ledNumber);
        ledComboBox->addItem(label, led->componentId());
        ledNumber++;
    }

    ledLegRadio->setEnabled(true);
}

void ConnectionSelectorDialog::populateResistorList(const QList<ResistorComponent*> &resistors)
{
    resistorComboBox->clear();

    if (resistors.isEmpty()) {
        resistorComboBox->addItem("No Resistors available", -1);
        resistorLeadRadio->setEnabled(false);
        return;
    }

    int resistorNumber = 1;
    for (ResistorComponent *resistor : resistors) {
        QString label = QString("Resistor %1 (%2Ω)").arg(resistorNumber).arg(resistor->resistance());
        resistorComboBox->addItem(label, resistor->componentId());
        resistorNumber++;
    }

    resistorLeadRadio->setEnabled(true);
}

void ConnectionSelectorDialog::populateButtonList(const QList<ButtonComponent*> &buttons)
{
    buttonComboBox->clear();

    if (buttons.isEmpty()) {
        buttonComboBox->addItem("No Buttons available", -1);
        buttonTerminalRadio->setEnabled(false);
        return;
    }

    int buttonNumber = 1;
    for (ButtonComponent *button : buttons) {
        QString label = QString("Button %1").arg(buttonNumber);
        buttonComboBox->addItem(label, button->componentId());
        buttonNumber++;
    }

    buttonTerminalRadio->setEnabled(true);
}
