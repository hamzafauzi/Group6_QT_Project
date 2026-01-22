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

// Forward declaration
class LEDComponent;

class ConnectionSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    enum ConnectionType {
        ARDUINO_PIN,
        LED_LEG
    };

    explicit ConnectionSelectorDialog(QWidget *parent = nullptr);

    ConnectionType getConnectionType() const;
    int getSelectedPin() const;
    int getSelectedLEDId() const;
    bool isPositiveLeg() const;

    void populateLEDList(const QList<LEDComponent*> &leds);

private:
    QRadioButton *arduinoPinRadio;
    QRadioButton *ledLegRadio;
    QComboBox *pinComboBox;
    QComboBox *ledComboBox;
    QComboBox *legTypeComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUI();
    void onConnectionTypeChanged();
};

#endif // CONNECTIONSELECTORDIALOG_H
