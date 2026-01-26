#include "add_new_element.h"
#include "ui_add_new_element.h"

Add_New_Element::Add_New_Element(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Add_New_Element)
{
    ui->setupUi(this);
}

Add_New_Element::~Add_New_Element()
{
    delete ui;
}
