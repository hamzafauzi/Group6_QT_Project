#include "addNewElement.h"
#include "ui_addNewElement.h"

Add_New_Element::Add_New_Element(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Add_New_Element)
{
    ui->setupUi(this);
    // Note: Qt's auto-connection will automatically connect slots named on_<objectName>_<signal>
    // So we don't need to manually connect here - the slots will be auto-connected
    // If we manually connect, it would create duplicate connections
}

Add_New_Element::~Add_New_Element()
{
    delete ui;
}
void Add_New_Element::on_pushButton_led_clicked()
{
    qDebug() << "LED button clicked";
    // Emit signal to notify that LED component was requested
    emit ledRequested();// macro to emit the signal defined in the header file
    qDebug() << "LED signal emitted";
    
    // Optionally close the window after selection
    this->close();
}

void Add_New_Element::on_pushButton_resistor_clicked()
{
    qDebug() << "Resistor button clicked";
    // Emit signal to notify that Resistor component was requested
    emit resistorRequested();//macro to emit the signal defined in the header file
    qDebug() << "Resistor signal emitted";
    
    // Optionally close the window after selection
    this->close();
}

void Add_New_Element::on_pushButton_button_clicked()
{
    qDebug() << "Button button clicked";
    // Emit signal to notify that Button component was requested
    emit buttonRequested();//macro to emit the signal defined in the header file
    qDebug() << "Button signal emitted";
    
    // Optionally close the window after selection
    this->close();
}
