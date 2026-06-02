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
    realTimeWidget = new QWidget(this);
    QVBoxLayout *rtLayout = new QVBoxLayout(realTimeWidget);
    rtLayout->addWidget(new QLabel("功能一：实时牌局信息抓取与同步。\n(此处可实现从抓包工具获取数据并展示当前牌桌状态)", realTimeWidget));
    
    // 2. 听牌训练 (何切)
    trainingWidget = new TrainingWidget(this);
    
    // 3. 对手分析
    analysisWidget = new QWidget(this);
    QVBoxLayout *anLayout = new QVBoxLayout(analysisWidget);
    anLayout->addWidget(new QLabel("功能三：对手风格分析与防守建议。\n(记录定缺倾向、倍率偏好、激进程度等)", analysisWidget));
    
    tabWidget->addTab(realTimeWidget, "实时辅助决策");
    tabWidget->addTab(trainingWidget, "何切训练");
    tabWidget->addTab(analysisWidget, "对手风格画像");
    
    setCentralWidget(tabWidget);
    resize(800, 600);
}
