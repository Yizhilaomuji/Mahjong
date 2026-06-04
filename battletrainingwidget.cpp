#include "battletrainingwidget.h"
#include <QFont>

BattleTrainingWidget::BattleTrainingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);

    // 页面 0: 菜单页 (两个按钮)
    m_menuPage = new QWidget(this);
    setupMenuPage();
    m_stack->addWidget(m_menuPage);

    // 页面 1: 战斗场 (CombatArenaWidget)，初始占位，点击按钮时替换
    m_arenaWidget = nullptr;
    m_stack->addWidget(new QWidget(this)); // placeholder

    mainLayout->addWidget(m_stack);
    m_stack->setCurrentIndex(0);
}

void BattleTrainingWidget::setupMenuPage() {
    QVBoxLayout *layout = new QVBoxLayout(m_menuPage);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("实战训练", m_menuPage);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 32px; font-weight: bold; color: #D77757; margin-bottom: 40px;");
    layout->addWidget(title);

    QLabel *subtitle = new QLabel("选择一种训练模式开始", m_menuPage);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-size: 14px; color: #888; margin-bottom: 40px;");
    layout->addWidget(subtitle);

    // 按钮容器
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(30);
    btnLayout->setAlignment(Qt::AlignCenter);

    // 实时辅助决策按钮
    QPushButton *realtimeBtn = new QPushButton("实时辅助决策", m_menuPage);
    realtimeBtn->setMinimumSize(200, 120);
    realtimeBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #D77757; color: white;"
        "  font-size: 18px; font-weight: bold;"
        "  border: 2px solid #c06648; border-radius: 12px;"
        "  padding: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e08868;"
        "}");
    connect(realtimeBtn, &QPushButton::clicked,
            this, &BattleTrainingWidget::onRealtimeAssist);

    // AI 模拟训练按钮
    QPushButton *aiBtn = new QPushButton("AI 模拟训练", m_menuPage);
    aiBtn->setMinimumSize(200, 120);
    aiBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #5769F7; color: white;"
        "  font-size: 18px; font-weight: bold;"
        "  border: 2px solid #4558e6; border-radius: 12px;"
        "  padding: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #6878ff;"
        "}");
    connect(aiBtn, &QPushButton::clicked,
            this, &BattleTrainingWidget::onAISimulation);

    btnLayout->addWidget(realtimeBtn);
    btnLayout->addWidget(aiBtn);
    layout->addLayout(btnLayout);

    // 功能说明
    QLabel *descLabel = new QLabel(
        "<br><b>实时辅助决策</b>: 支持网站/语音/视频/手动抓包，实时分析牌局并给出出牌建议<br>"
        "<b>AI 模拟训练</b>: 与 3 个 AI 对手对局，练习实战决策能力",
        m_menuPage);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet("font-size: 12px; color: #666;");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);
}

void BattleTrainingWidget::onRealtimeAssist() {
    if (m_arenaWidget) {
        m_stack->removeWidget(m_arenaWidget);
        delete m_arenaWidget;
    }
    m_arenaWidget = new CombatArenaWidget(ArenaMode::RealtimeAssist, this);
    connect(m_arenaWidget, &CombatArenaWidget::backRequested,
            this, &BattleTrainingWidget::onBackToMenu);
    m_stack->insertWidget(1, m_arenaWidget);
    m_stack->setCurrentIndex(1);
}

void BattleTrainingWidget::onAISimulation() {
    if (m_arenaWidget) {
        m_stack->removeWidget(m_arenaWidget);
        delete m_arenaWidget;
    }
    m_arenaWidget = new CombatArenaWidget(ArenaMode::AISimulation, this);
    connect(m_arenaWidget, &CombatArenaWidget::backRequested,
            this, &BattleTrainingWidget::onBackToMenu);
    m_stack->insertWidget(1, m_arenaWidget);
    m_stack->setCurrentIndex(1);
}

void BattleTrainingWidget::onBackToMenu() {
    m_stack->setCurrentIndex(0);
}
