#include "realtimeassistant.h"

RealTimeAssistant::RealTimeAssistant(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("<b>实时牌局辅助决策 (对局抓包模拟)</b>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QPushButton *captureBtn = new QPushButton("模拟抓包: 读取当前手牌", this);
    connect(captureBtn, &QPushButton::clicked, this, &RealTimeAssistant::simulateDataCapture);
    layout->addWidget(captureBtn);

    handLabel = new QLabel("当前手牌: 未连接", this);
    layout->addWidget(handLabel);

    suggestionBox = new QTextEdit(this);
    suggestionBox->setReadOnly(true);
    layout->addWidget(suggestionBox);
}

void RealTimeAssistant::simulateDataCapture()
{
    currentHand = MahjongLogic::generateTrainingHand();
    
    QString handStr = "检测到手牌: ";
    for (const auto& t : currentHand) {
        handStr += t.toString() + " ";
    }
    handLabel->setText(handStr);

    updateSuggestions();
}

void RealTimeAssistant::updateSuggestions()
{
    suggestionBox->clear();
    auto suggestions = MahjongLogic::getDiscardSuggestions(currentHand);
    
    if (suggestions.empty()) {
        suggestionBox->append("当前未听牌，建议根据向听数进一步组牌。");
        return;
    }

    suggestionBox->append("根据期望听数与番数，建议出牌方案：\n");
    
    for (const auto& pair : suggestions) {
        int maxFan = 0;
        QString waitsStr;
        for (const auto& w : pair.second) {
            waitsStr += w.toString() + " ";
            // 加入番数预测模拟
            std::vector<Tile> simulatedWin = currentHand;
            auto it = std::find(simulatedWin.begin(), simulatedWin.end(), pair.first);
            if (it != simulatedWin.end()) {
                simulatedWin.erase(it);
                simulatedWin.push_back(w);
                int fan = MahjongLogic::calculateFanTypes(simulatedWin);
                if (fan > maxFan) maxFan = fan;
            }
        }
        std::vector<Tile> afterDiscard = currentHand;
        MahjongLogic::removeTile(afterDiscard, pair.first);
        int waitTiles = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
        
        suggestionBox->append(QString("-> 打出 【%1】 听 %2 张牌: %3 (潜在最大倍率: %4番)")
                              .arg(pair.first.toString())
                              .arg(waitTiles)
                              .arg(waitsStr)
                              .arg(maxFan));
    }
    suggestionBox->append("\n*(此模块未来可挂载 WinPcap/代理拦截实现真实多平台抓包)*");
}