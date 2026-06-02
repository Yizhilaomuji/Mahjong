#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "trainingwidget.h"
#include "realtimeassistant.h"
#include "opponentanalysis.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    TrainingWidget *trainingWidget;
    RealTimeAssistant *realTimeWidget;
    OpponentAnalysis *analysisWidget;
    
    void setupUI();
};
#endif // MAINWINDOW_H
