#include "pininfodialog.h"
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

PinInfoDialog::PinInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Pin Information");
    setFixedSize(300, 150);
    
    // Create layout
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(20, 20, 20, 20);
    
    // Component name label
    m_componentLabel = new QLabel("Component: ", this);
    m_componentLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_layout->addWidget(m_componentLabel);
    
    // Pin name label
    m_pinLabel = new QLabel("Pin: ", this);
    m_pinLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_layout->addWidget(m_pinLabel);
    
    // Add some spacing
    m_layout->addSpacing(10);
    
    // Close button
    m_closeButton = new QPushButton("Close", this);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    m_layout->addWidget(m_closeButton);
}

void PinInfoDialog::setPinInfo(const QString &componentName, const QString &pinName)
{
    m_componentLabel->setText("Component: " + componentName);
    m_pinLabel->setText("Pin: " + pinName);
}
