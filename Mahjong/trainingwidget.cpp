#include "trainingwidget.h"

TrainingWidget::TrainingWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    
    QLabel *title = new QLabel("<b>何切题训练 (最大化听数)</b>", this);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    handLayout = new QHBoxLayout();
    mainLayout->addLayout(handLayout);

    infoLabel = new QLabel("点击下方按钮生成新手牌", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);

    QPushButton *newBtn = new QPushButton("生成新挑战", this);
    connect(newBtn, &QPushButton::clicked, this, &TrainingWidget::generateNewHand);
    mainLayout->addWidget(newBtn);
}

void TrainingWidget::generateNewHand()
{
    currentHand = MahjongLogic::generateTrainingHand();
    infoLabel->setText("请选择你要打出的牌，最大化听牌数：");
    updateUI();
}

void TrainingWidget::updateUI()
{
    // 清除旧按钮
    for (auto btn : tileButtons) {
        handLayout->removeWidget(btn);
        delete btn;
    }
    tileButtons.clear();

    // 创建新按钮
    for (size_t i = 0; i < currentHand.size(); ++i) {
        QPushButton *btn = new QPushButton(currentHand[i].toString(), this);
        btn->setFixedSize(50, 70);
        connect(btn, &QPushButton::clicked, [this, i]() { onTileClicked(i); });
        handLayout->addWidget(btn);
        tileButtons.append(btn);
    }
}

void TrainingWidget::onTileClicked(int index)
{
    Tile discarded = currentHand[index];
    
    // 获取系统的出牌与听牌面分析
    auto suggestions = MahjongLogic::getDiscardSuggestions(currentHand);
    
    QString resultText = QString("你打出了 %1。\n\n").arg(discarded.toString());
    
    // 找出最大听牌数
    size_t maxWaits = 0;
    for (const auto& pair : suggestions) {
        if (pair.second.size() > maxWaits) {
            maxWaits = pair.second.size();
        }
    }
    
    if (suggestions.find(discarded) != suggestions.end()) {
        const auto& waits = suggestions[discarded];
        resultText += QString("你打出这把牌后能听 %1 种牌: ").arg(waits.size());
        for (const auto& w : waits) {
            resultText += w.toString() + " ";
        }
        resultText += "\n";
        
        if (waits.size() == maxWaits) {
            resultText += "【正确！】你做出了最大听数的最优解。";
        } else {
            resultText += "【错误】存在更优解！\n";
        }
    } else {
        resultText += "【错误】打出这张牌后未下叫(未听牌)！\n";
    }
    
    // 列出所有可能听牌的选择
    if (maxWaits > 0 && (suggestions.find(discarded) == suggestions.end() || suggestions[discarded].size() < maxWaits)) {
        resultText += "\n最优出牌建议(听牌最多):\n";
        for (const auto& pair : suggestions) {
            if (pair.second.size() == maxWaits) {
                resultText += QString("打 %1 听 %2 种: ").arg(pair.first.toString()).arg(maxWaits);
                for (const auto& w : pair.second) {
                    resultText += w.toString() + " ";
                }
                resultText += "\n";
            }
        }
    }
    
    infoLabel->setText(resultText);
}
