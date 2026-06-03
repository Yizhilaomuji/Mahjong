#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("四川麻将 — 实战训练系统");
    tabWidget = new QTabWidget(this);

    // Tab 1: 实战训练
    battleTrainingWidget = new BattleTrainingWidget(this);
    tabWidget->addTab(battleTrainingWidget, "实战训练");

    // Tab 2: 模拟训练 (何切牌训练)
    trainingWidget = new TrainingWidget(this);
    tabWidget->addTab(trainingWidget, "模拟训练");

    setCentralWidget(tabWidget);
    resize(1200, 800);
}
