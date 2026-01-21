#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include "ledcomponent.h"
#include "buttoncomponent.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBuildClicked();
    void onRunClicked();

private:
    // UI Components
    QPushButton *buildButton;
    QPushButton *runButton;
    QTextEdit *codeEditor;
    QWidget *simulatorPanel;
    QTextBrowser *debugOutput;

    // Test components
    LEDComponent *testLED;
    ButtonComponent *testButton;

    // Helper methods
    void setupUI();
    void createControlPanel(QVBoxLayout *mainLayout);
    void createMiddleSection(QVBoxLayout *mainLayout);
    void createDebugPanel(QVBoxLayout *mainLayout);
    void logMessage(const QString &message);
    void testComponents();

    // Compilation flag
    bool isCompilationSuccessful;
};

#endif // MAINWINDOW_H
