#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include "addNewElement.h"
#include "arduinoCompiler.h"
#include "info.h"
class SimulatorScene;

namespace Ui
{
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
    void onConnectionLogged(const QString &message);
    void updateRunButtons();

private:
    bool m_buildSuccessful = false;// for build status tracking
    bool m_codeDirty = false;// to track if code has changed since last successful build
    Ui::StartWindow *ui;
    ArduinoCompiler *m_compiler;
    Add_New_Element *new_element;
    Info *info_window;
    SimulatorScene *m_scene;

    QPointF getRandomPosition();
};

#endif // STARTWINDOW_H
