#include "trainingwidget.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>

// --- 辅助函数：加载资源 (保持原样) ---
static QPixmap loadSvgTile(const QString &svgPath) {
    if (!QFile::exists(svgPath)) return QPixmap();
    QSvgRenderer renderer(svgPath);
    if (!renderer.isValid()) return QPixmap();
    QSize targetSize(50, 70);
    QPixmap pixmap(targetSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter, QRect(QPoint(0, 0), targetSize));
    return pixmap;
}

static QString suitColor(TileSuit suit) {
    switch (suit) {
        case TileSuit::Dot: return "#cc4444";
        case TileSuit::Bamboo: return "#44aa44";
        case TileSuit::Character: return "#4488cc";
        default: return "#777777";
    }
}

static QPixmap tileIcon(const Tile &tile) {
    QString suitName = (tile.suit == TileSuit::Dot) ? "dot" : 
                       (tile.suit == TileSuit::Bamboo) ? "bamboo" : "character";
    QString svgResourcePath = QStringLiteral(":/tiles/%1_%2.svg").arg(suitName).arg(tile.value);
    QPixmap pix = loadSvgTile(svgResourcePath);
    if (!pix.isNull()) return pix;

    // 此处省略原有的本地路径搜索逻辑，保持你原始代码中的搜索逻辑即可
    return QPixmap(); 
}

// --- TrainingWidget 主逻辑 ---

TrainingWidget::TrainingWidget(QWidget *parent) : QWidget(parent)
{
    // 设置主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // 1. 独立标题
    QLabel *title = new QLabel("<b>🀄 清一色听牌专项练习喵~ 🀄</b>", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; color: #2c3e50; font-family: 'Microsoft YaHei';");
    mainLayout->addWidget(title);

    // 2. 手牌显示区域
    handLayout = new QHBoxLayout();
    handLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(handLayout);

    // 3. 信息反馈区域
    infoLabel = new QLabel("正在初始化手牌...", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    infoLabel->setMinimumHeight(120);
    infoLabel->setStyleSheet("font-size: 16px; color: #555; background: #f9f9f9; border-radius: 10px; padding: 10px;");
    mainLayout->addWidget(infoLabel);

    // 4. 操作按钮区域
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    huButton = new QPushButton("啊哈哈哈，我已经和牌了喵！", this);
    huButton->setFixedHeight(50);
    huButton->setStyleSheet("QPushButton { background-color: #e67e22; color: white; border-radius: 5px; font-weight: bold; }"
                            "QPushButton:hover { background-color: #d35400; }");
    connect(huButton, &QPushButton::clicked, this, &TrainingWidget::onHuClicked);

    QPushButton *continueBtn = new QPushButton("下一题喵~（不许跳题喵！）", this);
    continueBtn->setFixedHeight(50);
    continueBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border-radius: 5px; font-weight: bold; }"
                               "QPushButton:hover { background-color: #219150; }");
    connect(continueBtn, &QPushButton::clicked, this, &TrainingWidget::onContinueClicked);

    btnLayout->addWidget(huButton);
    btnLayout->addWidget(continueBtn);
    mainLayout->addLayout(btnLayout);

    // 启动生成
    generateNewHand();
}

void TrainingWidget::generateNewHand()
{
    m_isAnswered = false; // 重置答题状态
    currentHand = MahjongLogic::generateTrainingHand(); // 确保逻辑类生成的是清一色
    infoLabel->setText("<b>【目标】</b> 请点击一张牌打出，使听牌枚数最多 <br>如果已经和牌，请点击和牌按钮喵~");
    updateUI();
}

void TrainingWidget::updateUI()
{
    // 清除旧按钮
    for (auto btn : tileButtons) {
        handLayout->removeWidget(btn);
        btn->deleteLater();
    }
    tileButtons.clear();

    // 创建手牌按钮
    for (size_t i = 0; i < currentHand.size(); ++i) {
        QPushButton *btn = new QPushButton(this);
        btn->setFixedSize(60, 85);
        QPixmap pix = tileIcon(currentHand[i]);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(QSize(54, 75));
        } else {
            btn->setText(currentHand[i].toString());
        }

        QString bgColor = suitColor(currentHand[i].suit);
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 2px solid #bdc3c7; border-radius: 5px; }"
            "QPushButton:hover { border: 2px solid #2c3e50; margin-top: -5px; }"
        ).arg(bgColor));

        connect(btn, &QPushButton::clicked, [this, i]() { onTileClicked(i); });
        handLayout->addWidget(btn);
        tileButtons.append(btn);
    }
}

void TrainingWidget::onTileClicked(int index)
{
    m_isAnswered = true; // 只要点击了，就算“做过题”了
    Tile discarded = currentHand[index];

    // 如果本身能和牌却去打牌
    if (MahjongLogic::canWin(currentHand)) {
        infoLabel->setText("<font color='#e74c3c'>杂鱼~ 和牌了你不点，你是不喜欢和牌吗~");
        return;
    }

    auto suggestions = MahjongLogic::getDiscardSuggestions(currentHand);
    QString resultText = QString("你打出了 <b>%1</b>。<br>").arg(discarded.toString());

    // 计算最大听牌张数逻辑...
    int maxWaits = 0;
    for (const auto& pair : suggestions) {
        std::vector<Tile> afterDiscard = currentHand;
        MahjongLogic::removeTile(afterDiscard, pair.first);
        int count = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
        if (count > maxWaits) maxWaits = count;
    }

    // 检查用户选择
    if (suggestions.find(discarded) != suggestions.end()) {
        const auto& waits = suggestions[discarded];
        std::vector<Tile> handAfter = currentHand;
        MahjongLogic::removeTile(handAfter, discarded);
        int waitCount = MahjongLogic::countRemainingWaitTiles(handAfter, waits);

        resultText += QString("听牌数: <font color='blue'>%1 张</font> (").arg(waitCount);
        for (const auto& w : waits) resultText += w.toString() + " ";
        resultText += ")<br>";

        resultText += (waitCount == maxWaits) ? "<font color='green'>强强！你真棒喵！</font>" : "<font color='#f39c12'>小杂鱼，太少了呢，再想想喵~</font>";
    } else {
        resultText += "<font color='red'>杂鱼~ 打这张可听不了牌喵~</font>";
    }

    resultText += "<br><br><b>[点击右侧按钮进入下一题]</b>";
    infoLabel->setText(resultText);
}

void TrainingWidget::onHuClicked()
{
    m_isAnswered = true; 
    if (MahjongLogic::canWin(currentHand)) {
        infoLabel->setText("<font color='green' size='5'>哎哟哟，还挺厉害，确实和牌了喵！</font><br>快快点击【下一题】继续喵！");
    } else {
        infoLabel->setText("<font color='red'>杂鱼~ 诈和可是要受惩罚的喵！");
    }
}

void TrainingWidget::onContinueClicked()
{
    // 如果一次都没点过（直接想跳过）
    if (!m_isAnswered) {
        infoLabel->setText("<font color='red' size='4'><b>你这个小杂鱼！</b></font><br>哪怕乱猜也要先点一张牌再走喵！不许直接逃跑喵！");
        return;
    }

    // 只要操作过，就直接生成下一题
    generateNewHand();
}