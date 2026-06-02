#include "opponentanalysis.h"

OpponentAnalysis::OpponentAnalysis(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("<b>对手风格画像与危险度防守分析</b>", this));

    QGridLayout *grid = new QGridLayout();
    
    QStringList styles = {"平衡型", "防守偏好", "进攻偏好 (做大牌)", "无脑杠"};
    QStringList suits = {"无", "缺筒", "缺条", "缺万"};

    for (int i = 0; i < 3; ++i) {
        QGroupBox *box = new QGroupBox(QString("对手 %1").arg(i + 1));
        QVBoxLayout *boxLayout = new QVBoxLayout(box);
        
        QComboBox *styleCombo = new QComboBox();
        styleCombo->addItems(styles);
        boxLayout->addWidget(new QLabel("打牌风格:"));
        boxLayout->addWidget(styleCombo);

        QComboBox *dingqueCombo = new QComboBox();
        dingqueCombo->addItems(suits);
        boxLayout->addWidget(new QLabel("检测到定缺:"));
        boxLayout->addWidget(dingqueCombo);

        grid->addWidget(box, 0, i);
    }
    layout->addLayout(grid);

    QPushButton *calcBtn = new QPushButton("计算全场炮牌(高危区间)", this);
    connect(calcBtn, &QPushButton::clicked, this, &OpponentAnalysis::calculateDanger);
    layout->addWidget(calcBtn);

    dangerOutput = new QLabel("高危炮牌预测:\n等待计算...", this);
    dangerOutput->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    dangerOutput->setMinimumHeight(100);
    layout->addWidget(dangerOutput);
}

void OpponentAnalysis::calculateDanger()
{
    // 模拟基于弃牌堆、自己手牌和对手定缺信息的贝叶斯/频数反算
    dangerOutput->setText("高危炮牌预测:\n"
                          "1. 【3筒】【6筒】 (对手1清一色/高番嫌疑)\n"
                          "2. 【5条】 (中心张，对手2已进入一向听危险层级)\n\n"
                          "防守建议：您可以优先打出场上已出现的熟张（如9万），切忌生张！");
}