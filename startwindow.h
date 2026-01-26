#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include "add_new_element.h"
#include "diagram.h"

class SimulatorScene;

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private slots:
    void onLedRequested();
    void onResistorRequested();
    void onButtonRequested();

private:
    Ui::StartWindow *ui;

    Add_New_Element* new_element;
    SimulatorScene* m_scene;
    Diagram* m_diagram;
    
    QPointF getRandomPosition();
};

#endif // STARTWINDOW_H
