#ifndef PINSELECTORDIALOG_H
#define PINSELECTORDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class PinSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PinSelectorDialog(QWidget *parent = nullptr);

    int getSelectedPin() const;

private:
    QComboBox *pinComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUI();
};

#endif // PINSELECTORDIALOG_H
