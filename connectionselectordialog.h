#ifndef CONNECTIONSELECTORDIALOG_H
#define CONNECTIONSELECTORDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

// Forward declarations
class LEDComponent;
class ResistorComponent;
class ButtonComponent;

class ConnectionSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    enum ConnectionType {
        ARDUINO_PIN,
        LED_LEG,
        RESISTOR_LEAD,
        BUTTON_TERMINAL
    };

    explicit ConnectionSelectorDialog(QWidget *parent = nullptr);

    ConnectionType getConnectionType() const;
    int getSelectedPin() const;
    int getSelectedLEDId() const;
    bool isPositiveLeg() const;
    int getSelectedResistorId() const;
    bool isLeftLead() const;
    int getSelectedButtonId() const;
    int getSelectedTerminalNumber() const;

    void populateLEDList(const QList<LEDComponent*> &leds);
    void populateResistorList(const QList<ResistorComponent*> &resistors);
    void populateButtonList(const QList<ButtonComponent*> &buttons);

private:
    QRadioButton *arduinoPinRadio;
    QRadioButton *ledLegRadio;
    QRadioButton *resistorLeadRadio;
    QRadioButton *buttonTerminalRadio;
    QComboBox *pinComboBox;
    QComboBox *ledComboBox;
    QComboBox *legTypeComboBox;
    QComboBox *resistorComboBox;
    QComboBox *leadTypeComboBox;
    QComboBox *buttonComboBox;
    QComboBox *terminalComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUI();
    void onConnectionTypeChanged();
};

#endif // CONNECTIONSELECTORDIALOG_H
