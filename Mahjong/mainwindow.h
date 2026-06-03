#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "battletrainingwidget.h"
#include "trainingwidget.h"

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
    BattleTrainingWidget *battleTrainingWidget;
    TrainingWidget *trainingWidget;

    void setupUI();
};
#endif // MAINWINDOW_H
