#ifndef ADD_NEW_ELEMENT_H
#define ADD_NEW_ELEMENT_H

#include <QMainWindow>

namespace Ui {
class Add_New_Element;
}

class Add_New_Element : public QMainWindow
{
    Q_OBJECT

public:
    explicit Add_New_Element(QWidget *parent = nullptr);
    ~Add_New_Element();

private:
    Ui::Add_New_Element *ui;
};

#endif // ADD_NEW_ELEMENT_H
