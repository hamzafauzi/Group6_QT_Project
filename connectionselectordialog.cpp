#include "connectionselectordialog.h"
#include "ledcomponent.h"

ConnectionSelectorDialog::ConnectionSelectorDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void ConnectionSelectorDialog::setupUI()
{
    setWindowTitle("Select Connection Target");
    setModal(true);
    resize(350, 280);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title label
    QLabel *titleLabel = new QLabel("What do you want to connect to?");
    titleLabel->setStyleSheet("font-size: 13px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Connection type selection
    arduinoPinRadio = new QRadioButton("Arduino Pin");
    ledLegRadio = new QRadioButton("LED Leg");
    arduinoPinRadio->setChecked(true);

    QVBoxLayout *radioLayout = new QVBoxLayout();
    radioLayout->addWidget(arduinoPinRadio);
    radioLayout->addWidget(ledLegRadio);
    mainLayout->addLayout(radioLayout);

    // Arduino Pin selection (shown by default)
    QLabel *pinLabel = new QLabel("Select Arduino Pin:");
    pinLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(pinLabel);

    pinComboBox = new QComboBox();
    pinComboBox->setStyleSheet(
        "QComboBox {"
        "   padding: 5px;"
        "   font-size: 11px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
        );

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

    // LED selection (hidden by default)
    QLabel *ledLabel = new QLabel("Select LED Component:");
    ledLabel->setStyleSheet("font-size: 11px; margin-top: 10px;");
    mainLayout->addWidget(ledLabel);

    ledComboBox = new QComboBox();
    ledComboBox->setStyleSheet(
        "QComboBox {"
        "   padding: 5px;"
        "   font-size: 11px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
        );
    ledComboBox->setEnabled(false);
    mainLayout->addWidget(ledComboBox);

    // Leg type selection
    QLabel *legLabel = new QLabel("Select Leg Type:");
    legLabel->setStyleSheet("font-size: 11px; margin-top: 5px;");
    mainLayout->addWidget(legLabel);

    legTypeComboBox = new QComboBox();
    legTypeComboBox->setStyleSheet(
        "QComboBox {"
        "   padding: 5px;"
        "   font-size: 11px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
        );
    legTypeComboBox->addItem("Long Leg (Positive +)", true);
    legTypeComboBox->addItem("Short Leg (Negative -)", false);
    legTypeComboBox->setEnabled(false);
    mainLayout->addWidget(legTypeComboBox);

    // Connect radio button signals
    connect(arduinoPinRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);
    connect(ledLegRadio, &QRadioButton::toggled, this, &ConnectionSelectorDialog::onConnectionTypeChanged);

    mainLayout->addSpacing(10);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    okButton = new QPushButton("OK");
    okButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   padding: 8px 20px;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        );
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    cancelButton = new QPushButton("Cancel");
    cancelButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   padding: 8px 20px;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}"
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
    } else {
        pinComboBox->setEnabled(false);
        ledComboBox->setEnabled(true);
        legTypeComboBox->setEnabled(true);
    }
}

ConnectionSelectorDialog::ConnectionType ConnectionSelectorDialog::getConnectionType() const
{
    return arduinoPinRadio->isChecked() ? ARDUINO_PIN : LED_LEG;
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
