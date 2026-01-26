#ifndef PININFODIALOG_H
#define PININFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class PinInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PinInfoDialog(QWidget *parent = nullptr);
    
    // Set the information to display
    void setPinInfo(const QString &componentName, const QString &pinName);

private:
    QLabel *m_componentLabel;
    QLabel *m_pinLabel;
    QPushButton *m_closeButton;
    QVBoxLayout *m_layout;
};

#endif // PININFODIALOG_H
