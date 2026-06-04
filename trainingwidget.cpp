#include "trainingwidget.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>

static QPixmap loadSvgTile(const QString &svgPath)
{
    if (!QFile::exists(svgPath)) {
        return QPixmap();
    }
    QSvgRenderer renderer(svgPath);
    if (!renderer.isValid()) {
        return QPixmap();
    }

    QSize targetSize(50, 70);
    QPixmap pixmap(targetSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter, QRect(QPoint(0, 0), targetSize));
    return pixmap;
}

static QString suitColor(TileSuit suit)
{
    switch (suit) {
        case TileSuit::Dot: return "#cc4444";
        case TileSuit::Bamboo: return "#44aa44";
        case TileSuit::Character: return "#4488cc";
    }
    return "#777777";
}

static QPixmap tileIcon(const Tile &tile)
{
    QString suitName;
    if (tile.suit == TileSuit::Dot) suitName = "dot";
    else if (tile.suit == TileSuit::Bamboo) suitName = "bamboo";
    else suitName = "character";

    QString svgResourcePath = QStringLiteral(":/tiles/%1_%2.svg").arg(suitName).arg(tile.value);
    QPixmap pix = loadSvgTile(svgResourcePath);
    if (!pix.isNull()) {
        return pix;
    }

    QString pngResourcePath = QStringLiteral(":/tiles/%1_%2.png").arg(suitName).arg(tile.value);
    pix = QPixmap(pngResourcePath);
    if (!pix.isNull()) {
        return pix;
    }

    QString appDir = QCoreApplication::applicationDirPath();
    QChar suitSuffix = (suitName == "character") ? QChar('m')
                        : (suitName == "bamboo") ? QChar('s')
                        : QChar('p');
    QString fileName = QString::number(tile.value) + suitSuffix;

    QStringList searchDirs = {
        QDir::cleanPath(appDir + "/ai pictures"),
        QDir::cleanPath(appDir + "/../ai pictures"),
        QDir::cleanPath(appDir + "/../../ai pictures"),
        QDir::cleanPath(appDir + "/../pictures"),
        QDir::cleanPath(appDir + "/../tiles")
    };

    for (const QString &dir : searchDirs) {
        QString localSvgPath = QDir::cleanPath(dir + "/" + fileName + ".svg");
        pix = loadSvgTile(localSvgPath);
        if (!pix.isNull()) {
            return pix;
        }

        QString localPngPath = QDir::cleanPath(dir + "/" + fileName + ".png");
        if (QFile::exists(localPngPath)) {
            return QPixmap(localPngPath);
        }
    }

    return QPixmap();
}

TrainingWidget::TrainingWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    
    QLabel *title = new QLabel("<b>清一色训练喵~</b>", this);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    handLayout = new QHBoxLayout();
    mainLayout->addLayout(handLayout);

    infoLabel = new QLabel("点击下方按钮生成新手牌喵~", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);

    QPushButton *continueBtn = new QPushButton("继续喵~", this);
    connect(continueBtn, &QPushButton::clicked, this, &TrainingWidget::onContinueClicked);
    mainLayout->addWidget(continueBtn);

    huButton = new QPushButton("啊哈哈哈，已经和牌了喵~", this);
    connect(huButton, &QPushButton::clicked, this, &TrainingWidget::onHuClicked);
    mainLayout->addWidget(huButton);

    // 启动时直接生成一手牌，避免进入页面后空白
    generateNewHand();
}

void TrainingWidget::generateNewHand()
{
    currentHand = MahjongLogic::generateTrainingHand();
    infoLabel->setText("请选择你要打出的牌，最大化听牌数；如果已满足和牌，请点击“和牌”喵~");
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
        QPushButton *btn = new QPushButton(this);
        btn->setFixedSize(56, 82);
        QPixmap pix = tileIcon(currentHand[i]);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(QSize(50, 70));
            btn->setText("");
        } else {
            btn->setText(currentHand[i].toString());
        }

        QString bgColor = suitColor(currentHand[i].suit);
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: white; border: 1px solid #444; "
            "border-radius: 8px; font-size: 14px; }"
            "QPushButton:hover { border: 1px solid #fff; }"
        ).arg(bgColor));

        connect(btn, &QPushButton::clicked, [this, i]() { onTileClicked(i); });
        handLayout->addWidget(btn);
        tileButtons.append(btn);
    }

    huButton->setEnabled(true);
}

void TrainingWidget::onTileClicked(int index)
{
    Tile discarded = currentHand[index];

    if (MahjongLogic::canWin(currentHand)) {
        infoLabel->setText("杂鱼~你这家伙怎么不和牌呢~哦吼吼吼~\n");
        return;
    }
    
    // 获取系统的出牌与听牌面分析
    auto suggestions = MahjongLogic::getDiscardSuggestions(currentHand);
    
    QString resultText = QString("你打出了 %1。\n").arg(discarded.toString());
    
    // 找出最大听牌张数
    int maxWaits = 0;
    for (const auto& pair : suggestions) {
        std::vector<Tile> afterDiscard = currentHand;
        MahjongLogic::removeTile(afterDiscard, pair.first);
        int count = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
        if (count > maxWaits) {
            maxWaits = count;
        }
    }
    
    if (suggestions.find(discarded) != suggestions.end()) {
        const auto& waits = suggestions[discarded];
        std::vector<Tile> hand13 = currentHand;
        MahjongLogic::removeTile(hand13, discarded);
        int waitTiles = MahjongLogic::countRemainingWaitTiles(hand13, waits);
        resultText += QString("你打出这把牌后能听 %1 张牌: \n").arg(waitTiles);
        for (const auto& w : waits) {
            resultText += w.toString() + " ";
        }
        resultText += "\n";
        
        if (waitTiles == maxWaits) {
            resultText += "强强！真棒喵~\n";
        } else {
            resultText += "哦吼吼吼~还有更好的选择喵！\n";
        }
    } else {
        resultText += "杂鱼~你是不喜欢听牌吗~\n";
    }
    
    // 列出所有可能听牌的选择
    if (maxWaits > 0) {
        int currentDiscardCount = 0;
        auto it = suggestions.find(discarded);
        if (it != suggestions.end()) {
            std::vector<Tile> currentDiscardHand = currentHand;
            MahjongLogic::removeTile(currentDiscardHand, discarded);
            currentDiscardCount = MahjongLogic::countRemainingWaitTiles(currentDiscardHand, it->second);
        }
        if (suggestions.find(discarded) == suggestions.end() || currentDiscardCount < maxWaits) {
            resultText += "\n最优出牌建议(听牌最多):\n";
            for (const auto& pair : suggestions) {
                std::vector<Tile> afterDiscard = currentHand;
                MahjongLogic::removeTile(afterDiscard, pair.first);
                int waitTiles = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
                if (waitTiles == maxWaits) {
                    resultText += QString("打 %1 听 %2 张: ").arg(pair.first.toString()).arg(waitTiles);
                    for (const auto& w : pair.second) {
                        resultText += w.toString() + " ";
                    }
                    resultText += "\n";
                }
            }
        }
    }

    infoLabel->setText(resultText);
}

void TrainingWidget::onContinueClicked()
{
    if (!MahjongLogic::canWin(currentHand)) {
        infoLabel->setText("你这个小杂鱼，没做完题不能逃跑喵！");
        return;
    }
    generateNewHand();
}

void TrainingWidget::onHuClicked()
{
    if (MahjongLogic::canWin(currentHand)) {
        infoLabel->setText("你做对了喵！真的和了喵！请点击'继续'喵~\n");
    } else {
        infoLabel->setText("杂鱼~诈和是要被惩罚的哦~\n");
    }
}
