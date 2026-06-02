#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>

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
    tabWidget = new QTabWidget(this);
    
    // 1. 实时牌局感知
    realTimeWidget = new RealTimeAssistant(this);
    
    // 2. 听牌训练 (何切)
    trainingWidget = new TrainingWidget(this);
    
    // 3. 对手分析
    analysisWidget = new OpponentAnalysis(this);
    
    tabWidget->addTab(realTimeWidget, "实时辅助决策(抓包)");
    tabWidget->addTab(trainingWidget, "何切训练(听牌最大化)");
    tabWidget->addTab(analysisWidget, "防守与画像(炮牌预警)");
    
    setCentralWidget(tabWidget);
    resize(800, 600);
}
