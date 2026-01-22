#include "pinselectordialog.h"

PinSelectorDialog::PinSelectorDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void PinSelectorDialog::setupUI()
{
    setWindowTitle("Select Arduino Pin");
    setModal(true);
    resize(300, 180);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Label
    QLabel *label = new QLabel("Select which Arduino pin to connect:");
    label->setStyleSheet("font-size: 12px; margin-bottom: 10px;");
    mainLayout->addWidget(label);

    // Pin combo box
    pinComboBox = new QComboBox();
    pinComboBox->setStyleSheet(
        "QComboBox {"
        "   padding: 5px;"
        "   font-size: 12px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
        );

    // Add digital pins (0-13)
    for (int i = 0; i <= 13; i++) {
        pinComboBox->addItem(QString("Digital Pin %1").arg(i), i);
    }

    // Add separator
    pinComboBox->insertSeparator(pinComboBox->count());

    // Add analog pins (A0-A5 mapped to 14-19)
    for (int i = 0; i <= 5; i++) {
        pinComboBox->addItem(QString("Analog Pin A%1").arg(i), 14 + i);
    }

    // Add separator
    pinComboBox->insertSeparator(pinComboBox->count());

    // Add power pins (20-24)
    pinComboBox->addItem("RESET", 20);
    pinComboBox->addItem("3.3V", 21);
    pinComboBox->addItem("5V", 22);
    pinComboBox->addItem("GND", 23);
    pinComboBox->addItem("VIN", 24);

    mainLayout->addWidget(pinComboBox);

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

int PinSelectorDialog::getSelectedPin() const
{
    return pinComboBox->currentData().toInt();
}
