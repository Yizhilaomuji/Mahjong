#include "simulationwidget.h"
#include <QScrollArea>
#include <QSplitter>
#include <QSvgRenderer>
#include <QPainter>
#include <QBuffer>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

static QPixmap loadSvgTile(const QString &svgPath) {
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

static QPixmap tileIcon(const Tile &tile) {
    QChar suitSuffix = tile.suit == TileSuit::Character ? QChar('m')
                        : tile.suit == TileSuit::Bamboo ? QChar('s')
                        : QChar('p');
    QString appDir = QCoreApplication::applicationDirPath();
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
        QPixmap pix = loadSvgTile(localSvgPath);
        if (!pix.isNull()) {
            return pix;
        }

        QString localPngPath = QDir::cleanPath(dir + "/" + fileName + ".png");
        if (QFile::exists(localPngPath)) {
            pix = QPixmap(localPngPath);
            if (!pix.isNull()) {
                return pix.scaled(50, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    QString suitName = tile.suit == TileSuit::Character ? "character"
                        : tile.suit == TileSuit::Bamboo ? "bamboo"
                        : "dot";
    QString svgResourcePath = QStringLiteral(":/tiles/%1_%2.svg").arg(suitName).arg(tile.value);
    QPixmap pix = loadSvgTile(svgResourcePath);
    if (!pix.isNull()) {
        return pix;
    }

    QString pngResourcePath = QStringLiteral(":/tiles/%1_%2.png").arg(suitName).arg(tile.value);
    pix = QPixmap(pngResourcePath);
    if (!pix.isNull()) {
        return pix.scaled(50, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return QPixmap();
}

SimulationWidget::SimulationWidget(QWidget *parent) : QWidget(parent) {
    m_engine = new GameEngine(this);
    m_ai = new AIPlayer(this);
    m_aiTimer = new QTimer(this);
    m_aiTimer->setSingleShot(true);
    m_aiTimer->setInterval(800); // AI 延迟使得看起来像在思考
    connect(m_aiTimer, &QTimer::timeout, this, &SimulationWidget::onAITimerTick);

    setupUI();

    // 连接引擎信号
    connect(m_engine, &GameEngine::gameStarted, this, &SimulationWidget::onGameStarted);
    connect(m_engine, &GameEngine::turnChanged, this, &SimulationWidget::onTurnChanged);
    connect(m_engine, &GameEngine::playerDiscarded, this, &SimulationWidget::onPlayerDiscarded);
    connect(m_engine, &GameEngine::playerPonged, this, &SimulationWidget::onPlayerPonged);
    connect(m_engine, &GameEngine::playerKonged, this, &SimulationWidget::onPlayerKonged);
    connect(m_engine, &GameEngine::playerWon, this, &SimulationWidget::onPlayerWon);
    connect(m_engine, &GameEngine::dingqueSet, this, &SimulationWidget::onDingqueSet);
    connect(m_engine, &GameEngine::gameOver, this, &SimulationWidget::onGameOver);
    connect(m_engine, &GameEngine::logMessage, this, &SimulationWidget::onLogMessage);
}

void SimulationWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 顶部控制栏 =====
    QHBoxLayout *topBar = new QHBoxLayout();

    QPushButton *newGameBtn = new QPushButton("新游戏", this);
    connect(newGameBtn, &QPushButton::clicked, this, &SimulationWidget::onNewGame);
    topBar->addWidget(newGameBtn);

    topBar->addStretch();

    for (int i = 1; i < 4; ++i) {
        m_autoPilot[i] = new QCheckBox(QString("托管 P%1").arg(i + 1), this);
        m_autoPilot[i]->setChecked(true);
        connect(m_autoPilot[i], &QCheckBox::toggled, this, [this, i](bool checked) {
            onAutoPilotToggled(i, checked);
        });
        topBar->addWidget(m_autoPilot[i]);
    }

    topBar->addStretch();

    m_wallLabel = new QLabel("牌墙: --", this);
    topBar->addWidget(m_wallLabel);

    mainLayout->addLayout(topBar);

    // ===== 中间牌桌区域 =====
    QGridLayout *tableGrid = new QGridLayout();
    tableGrid->setSpacing(8);

    // 对手2 (上)
    m_playerBoxes[2] = createPlayerBox(2);
    tableGrid->addWidget(m_playerBoxes[2], 0, 1);

    // 对手3 (左)
    m_playerBoxes[3] = createPlayerBox(3);
    tableGrid->addWidget(m_playerBoxes[3], 1, 0);

    // 中央区域
    QGroupBox *centerBox = new QGroupBox(this);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerBox);
    m_lastDiscardLabel = new QLabel("最后打出:\n---", this);
    m_lastDiscardLabel->setAlignment(Qt::AlignCenter);
    m_lastDiscardLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    centerLayout->addWidget(m_lastDiscardLabel);

    m_statusLabel = new QLabel("点击「新游戏」开始", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 14px;");
    centerLayout->addWidget(m_statusLabel);

    // 定缺面板
    m_dingquePanel = new QWidget(this);
    QHBoxLayout *dingqueLayout = new QHBoxLayout(m_dingquePanel);
    m_dingqueDotBtn = new QPushButton("缺筒", this);
    m_dingqueDotBtn->setStyleSheet("background-color: #cc4444; color: white; font-size: 16px; padding: 10px;");
    m_dingqueBambooBtn = new QPushButton("缺条", this);
    m_dingqueBambooBtn->setStyleSheet("background-color: #44aa44; color: white; font-size: 16px; padding: 10px;");
    m_dingqueCharBtn = new QPushButton("缺万", this);
    m_dingqueCharBtn->setStyleSheet("background-color: #4444cc; color: white; font-size: 16px; padding: 10px;");
    connect(m_dingqueDotBtn, &QPushButton::clicked, this, [this]() { onDingqueClicked(TileSuit::Dot); });
    connect(m_dingqueBambooBtn, &QPushButton::clicked, this, [this]() { onDingqueClicked(TileSuit::Bamboo); });
    connect(m_dingqueCharBtn, &QPushButton::clicked, this, [this]() { onDingqueClicked(TileSuit::Character); });
    dingqueLayout->addWidget(m_dingqueDotBtn);
    dingqueLayout->addWidget(m_dingqueBambooBtn);
    dingqueLayout->addWidget(m_dingqueCharBtn);
    m_dingquePanel->hide();
    centerLayout->addWidget(m_dingquePanel);

    // 操作按钮面板（碰/杠/胡/过）
    m_actionPanel = new QWidget(this);
    QHBoxLayout *actionLayout = new QHBoxLayout(m_actionPanel);
    m_pongBtn = new QPushButton("碰", this);
    m_pongBtn->setStyleSheet("background-color: #ddaa00; color: white; font-size: 14px; padding: 8px;");
    connect(m_pongBtn, &QPushButton::clicked, this, &SimulationWidget::onPongClicked);

    m_kongBtn = new QPushButton("杠", this);
    m_kongBtn->setStyleSheet("background-color: #8888ff; color: white; font-size: 14px; padding: 8px;");
    connect(m_kongBtn, &QPushButton::clicked, this, &SimulationWidget::onKongClicked);

    m_selfKongBtn = new QPushButton("暗杠/加杠", this);
    m_selfKongBtn->setStyleSheet("background-color: #6666cc; color: white; font-size: 14px; padding: 8px;");
    connect(m_selfKongBtn, &QPushButton::clicked, this, &SimulationWidget::onSelfKongClicked);

    m_winBtn = new QPushButton("胡!", this);
    m_winBtn->setStyleSheet("background-color: #cc4444; color: white; font-size: 14px; padding: 8px; font-weight: bold;");
    connect(m_winBtn, &QPushButton::clicked, this, &SimulationWidget::onWinClicked);

    m_passBtn = new QPushButton("过", this);
    m_passBtn->setStyleSheet("background-color: #888888; color: white; font-size: 14px; padding: 8px;");
    connect(m_passBtn, &QPushButton::clicked, this, &SimulationWidget::onPassClicked);

    actionLayout->addWidget(m_winBtn);
    actionLayout->addWidget(m_kongBtn);
    actionLayout->addWidget(m_pongBtn);
    actionLayout->addWidget(m_selfKongBtn);
    actionLayout->addWidget(m_passBtn);
    m_actionPanel->hide();
    centerLayout->addWidget(m_actionPanel);

    centerLayout->addStretch();
    tableGrid->addWidget(centerBox, 1, 1);

    // 对手1 (右)
    m_playerBoxes[1] = createPlayerBox(1);
    tableGrid->addWidget(m_playerBoxes[1], 1, 2);

    // 玩家 (下)
    m_playerBoxes[0] = createPlayerBox(0);
    tableGrid->addWidget(m_playerBoxes[0], 2, 1);

    mainLayout->addLayout(tableGrid);

    // ===== 日志区域 =====
    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(120);
    m_logView->setPlaceholderText("游戏日志...");
    mainLayout->addWidget(m_logView);
}

QGroupBox *SimulationWidget::createPlayerBox(int index) {
    QGroupBox *box = new QGroupBox(
        index == 0 ? "你 (玩家1)" : QString("对手 %1").arg(index + 1),
        this
    );
    QVBoxLayout *layout = new QVBoxLayout(box);

    // 定缺/状态
    m_dingqueLabels[index] = new QLabel("定缺: --", this);
    layout->addWidget(m_dingqueLabels[index]);

    m_statusLabels[index] = new QLabel("", this);
    layout->addWidget(m_statusLabels[index]);

    // 面子（碰/杠）
    m_meldLabels[index] = new QLabel("", this);
    m_meldLabels[index]->setWordWrap(true);
    layout->addWidget(m_meldLabels[index]);

    // 手牌
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setMaximumHeight(70);
    QWidget *handContainer = new QWidget(this);
    m_handLayouts[index] = new QHBoxLayout(handContainer);
    m_handLayouts[index]->setSpacing(2);
    m_handLayouts[index]->setContentsMargins(2, 2, 2, 2);
    m_handLayouts[index]->addStretch();
    scroll->setWidget(handContainer);
    layout->addWidget(scroll);

    // 弃牌
    m_discardLabels[index] = new QLabel("弃牌: ", this);
    m_discardLabels[index]->setWordWrap(true);
    layout->addWidget(m_discardLabels[index]);

    return box;
}

void SimulationWidget::onNewGame() {
    m_engine->startNewGame();
}

void SimulationWidget::onDingqueClicked(TileSuit suit) {
    m_engine->setDingque(0, suit);
    hideDingquePanel();
}

void SimulationWidget::onHandTileClicked(int index) {
    if (!isHumanTurn()) return;
    if (m_engine->phase() != GameEngine::Playing) return;
    if (index < 0 || index >= static_cast<int>(m_displayHands[0].size())) return;

    Tile tile = m_displayHands[0][index];
    m_engine->playerDiscard(tile);
}

void SimulationWidget::onPongClicked() {
    m_engine->playerPong(0);
    hideDingquePanel();
    refreshAllPlayerUI();
    refreshActionButtons();
}

void SimulationWidget::onKongClicked() {
    m_engine->playerKong(0);
    hideDingquePanel();
    refreshAllPlayerUI();
    refreshActionButtons();
}

void SimulationWidget::onSelfKongClicked() {
    // 需要确定哪种杠类型和哪张牌
    // 简化：遍历手牌找暗杠机会
    const auto &hand = m_engine->playerState(0).hand;
    std::vector<int> counts = MahjongLogic::tilesToCounts(hand);
    for (int i = 0; i < 30; ++i) {
        if (counts[i] >= 4) {
            Tile tile = {static_cast<TileSuit>(i / 10), i % 10};
            m_engine->playerSelfKong(0, tile, GameEngine::HiddenKong);
            refreshAllPlayerUI();
            refreshActionButtons();
            return;
        }
    }
    // 检查加杠
    const auto &melds = m_engine->playerState(0).melds;
    for (const auto &meld : melds) {
        if (meld.size() == 3) {
            for (const auto &t : hand) {
                if (t == meld[0]) {
                    m_engine->playerSelfKong(0, t, GameEngine::AddKong);
                    refreshAllPlayerUI();
                    refreshActionButtons();
                    return;
                }
            }
        }
    }
}

void SimulationWidget::onWinClicked() {
    if (m_engine->canSelfWin(0)) {
        m_engine->playerSelfWin(0);
    } else if (m_engine->canWinOnDiscard(0)) {
        m_engine->playerWinOnDiscard(0);
    }
    hideDingquePanel();
    refreshAllPlayerUI();
    refreshActionButtons();
}

void SimulationWidget::onPassClicked() {
    // 不做任何操作，AI 会检查是否还有其他响应者
    // 简单处理：推进到下一回合
    m_engine->advanceTurn();
    hideDingquePanel();
    refreshAllPlayerUI();
    refreshActionButtons();
}

void SimulationWidget::onAutoPilotToggled(int player, bool enabled) {
    m_engine->setHuman(player, !enabled);
    if (enabled && m_engine->phase() == GameEngine::Playing) {
        // 如果启用了托管且轮到该AI，触发AI回合
        if (m_engine->currentPlayer() == player) {
            m_aiTimer->start();
        }
    }
}

void SimulationWidget::onAITimerTick() {
    runAITurn();
}

// ===== Engine signal handlers =====

void SimulationWidget::onGameStarted() {
    m_logView->clear();
    hideDingquePanel();
    m_actionPanel->hide();
    refreshAllPlayerUI();

    // AI 自动定缺
    for (int i = 1; i < 4; ++i) {
        TileSuit dq = m_ai->chooseDingque(m_engine->playerState(i).hand);
        m_engine->setDingque(i, dq);
    }

    // 人类玩家需要手动定缺
    showDingquePanel();
    m_statusLabel->setText("请选择定缺花色");
}

void SimulationWidget::onTurnChanged(int player) {
    refreshAllPlayerUI();

    if (player == 0) {
        m_statusLabel->setText("你的回合: 选择一张手牌打出");
    } else {
        m_statusLabel->setText(QString("对手%1 的回合...").arg(player + 1));
    }

    // 检查是否是当前玩家的回合，非人类且未出牌时触发 AI
    if (player != 0 && !m_engine->playerState(player).isHuman) {
        const auto &hand = m_engine->playerState(player).hand;
        // 摸牌后检查是否能自摸/暗杠
        if (m_engine->canSelfWin(player)) {
            m_engine->playerSelfWin(player);
        } else if (m_engine->canSelfKong(player)) {
            // AI 判断是否要杠
            const auto &melds = m_engine->playerState(player).melds;
            if (m_ai->shouldSelfKong(m_engine->playerState(player).hand, melds)) {
                // 找杠的牌
                std::vector<int> counts = MahjongLogic::tilesToCounts(hand);
                for (int i = 0; i < 30; ++i) {
                    if (counts[i] >= 4) {
                        Tile tile = {static_cast<TileSuit>(i / 10), i % 10};
                        m_engine->playerSelfKong(player, tile, GameEngine::HiddenKong);
                        refreshAllPlayerUI();
                        // 杠后还要出牌，用定时器
                        m_aiTimer->start();
                        return;
                    }
                }
                // 加杠
                for (const auto &meld : melds) {
                    if (meld.size() == 3) {
                        for (const auto &t : hand) {
                            if (t == meld[0]) {
                                m_engine->playerSelfKong(player, t, GameEngine::AddKong);
                                refreshAllPlayerUI();
                                m_aiTimer->start();
                                return;
                            }
                        }
                    }
                }
            }
        }
        // 出牌
        m_aiTimer->start();
    }
}

void SimulationWidget::onPlayerDiscarded(int player, Tile tile) {
    Q_UNUSED(tile);
    refreshAllPlayerUI();

    // 如果人类玩家需要响应，显示操作按钮
    if (player != 0) {
        refreshActionButtons();
    } else {
        // 人类出牌后检查是否有 AI 要响应
        // 由 checkResponseActions 内部处理
        m_aiTimer->start();
    }
}

void SimulationWidget::onPlayerPonged(int player, Tile tile) {
    Q_UNUSED(tile);
    refreshAllPlayerUI();
    // 碰牌后该玩家需要出牌
    if (player != 0 && !m_engine->playerState(player).isHuman) {
        m_aiTimer->start();
    }
}

void SimulationWidget::onPlayerKonged(int player, Tile tile, GameEngine::KongType kongType) {
    Q_UNUSED(tile);
    Q_UNUSED(kongType);
    refreshAllPlayerUI();
    // 杠后该玩家需要出牌
    if (player != 0 && !m_engine->playerState(player).isHuman) {
        m_aiTimer->start();
    }
}

void SimulationWidget::onPlayerWon(int player, bool selfDrawn, int fan) {
    refreshAllPlayerUI();
    m_statusLabel->setText(QString("玩家%1 %2! (%3番)")
        .arg(player + 1)
        .arg(selfDrawn ? "自摸" : "胡")
        .arg(fan));

    // 血战：如果游戏没结束，继续
    if (m_engine->phase() == GameEngine::Playing) {
        m_aiTimer->start();
    }
}

void SimulationWidget::onDingqueSet(int player, TileSuit suit) {
    QString name = suit == TileSuit::Dot ? "缺筒" : (suit == TileSuit::Bamboo ? "缺条" : "缺万");
    m_dingqueLabels[player]->setText(name);
}

void SimulationWidget::onGameOver() {
    m_statusLabel->setText("对局结束! 点击「新游戏」再来一局");
    m_actionPanel->hide();
    hideDingquePanel();
    refreshAllPlayerUI();
}

void SimulationWidget::onLogMessage(const QString &msg) {
    m_logView->append(msg);
}

// ===== UI 刷新 =====

void SimulationWidget::refreshAllPlayerUI() {
    for (int i = 0; i < 4; ++i) {
        refreshPlayerUI(i);
    }
    m_wallLabel->setText(QString("牌墙: %1").arg(m_engine->tilesRemaining()));

    // 更新最后打出的牌
    if (m_engine->lastDiscardPlayer() >= 0) {
        m_lastDiscardLabel->setText(
            QString("最后打出:\n%1\n(玩家%2)")
                .arg(tileToDisplay(m_engine->lastDiscard()))
                .arg(m_engine->lastDiscardPlayer() + 1)
        );
    }
}

void SimulationWidget::refreshPlayerUI(int player) {
    const PlayerState &ps = m_engine->playerState(player);

    // 清空手牌布局
    QHBoxLayout *hl = m_handLayouts[player];
    while (hl->count() > 0) {
        QLayoutItem *item = hl->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_handButtons[player].clear();
    m_displayHands[player] = ps.hand;

    // 渲染手牌
    if (player == 0) {
        // 人类玩家：可点击按钮
        bool isTurn = isHumanTurn() && m_engine->phase() == GameEngine::Playing;
        for (size_t i = 0; i < ps.hand.size(); ++i) {
            QPushButton *btn = new QPushButton(this);
            QPixmap pix = tileIcon(ps.hand[i]);
            if (!pix.isNull()) {
                btn->setIcon(QIcon(pix));
                btn->setIconSize(QSize(50, 70));
                btn->setText("");
            } else {
                btn->setText(tileToDisplay(ps.hand[i]));
            }
            btn->setFixedSize(56, 82);
            btn->setEnabled(isTurn);
            int idx = static_cast<int>(i);
            connect(btn, &QPushButton::clicked, this, [this, idx]() {
                onHandTileClicked(idx);
            });
            hl->addWidget(btn);
            m_handButtons[player].append(btn);
        }
    } else {
        // AI 玩家：显示牌面（不隐藏，可以看到）
        for (size_t i = 0; i < ps.hand.size(); ++i) {
            QPushButton *btn = new QPushButton(this);
            QPixmap pix = tileIcon(ps.hand[i]);
            if (!pix.isNull()) {
                btn->setIcon(QIcon(pix));
                btn->setIconSize(QSize(40, 56));
                btn->setText("");
            } else {
                btn->setText(tileToDisplay(ps.hand[i]));
            }
            btn->setFixedSize(44, 64);
            btn->setEnabled(false);
            hl->addWidget(btn);
            m_handButtons[player].append(btn);
        }
    }
    hl->addStretch();

    // 面子（碰/杠）
    QString meldStr;
    for (const auto &meld : ps.melds) {
        meldStr += "[";
        for (const auto &t : meld) {
            meldStr += tileToDisplay(t);
        }
        meldStr += "] ";
    }
    m_meldLabels[player]->setText(meldStr.isEmpty() ? "" : "面子: " + meldStr);

    // 弃牌
    QString discardStr = "弃牌: ";
    for (const auto &t : ps.discards) {
        discardStr += tileToDisplay(t) + " ";
    }
    m_discardLabels[player]->setText(discardStr);

    // 状态
    QString statusText;
    if (ps.hasWon) {
        statusText = "【已胡】";
        m_statusLabels[player]->setStyleSheet("color: red; font-weight: bold;");
    } else if (m_engine->phase() == GameEngine::Playing && m_engine->currentPlayer() == player) {
        statusText = "← 当前回合";
        m_statusLabels[player]->setStyleSheet("color: #ddaa00; font-weight: bold;");
    } else {
        m_statusLabels[player]->setStyleSheet("");
    }
    m_statusLabels[player]->setText(statusText);

    // 玩家框样式
    if (ps.hasWon) {
        m_playerBoxes[player]->setStyleSheet("QGroupBox { border: 2px solid red; }");
    } else if (m_engine->phase() == GameEngine::Playing && m_engine->currentPlayer() == player) {
        m_playerBoxes[player]->setStyleSheet("QGroupBox { border: 2px solid #ddaa00; }");
    } else {
        m_playerBoxes[player]->setStyleSheet("");
    }
}

void SimulationWidget::refreshActionButtons() {
    if (m_engine->phase() != GameEngine::Playing) {
        m_actionPanel->hide();
        return;
    }

    int lp = m_engine->lastDiscardPlayer();
    if (lp < 0 || lp == 0) {
        // 没有其他玩家刚出牌，或者自己刚出牌
        m_actionPanel->hide();
        return;
    }

    // 有其他玩家出牌，检查人类能否响应
    bool canP = m_engine->canPong(0);
    bool canK = m_engine->canExposedKong(0);
    bool canW = m_engine->canWinOnDiscard(0);

    if (canP || canK || canW) {
        m_pongBtn->setVisible(canP);
        m_kongBtn->setVisible(canK);
        m_winBtn->setVisible(canW);
        m_selfKongBtn->hide(); // 响应出牌时不能暗杠
        m_actionPanel->show();
    } else {
        m_actionPanel->hide();
        // 人类不能响应，自动跳过
        m_engine->advanceTurn();
        refreshAllPlayerUI();
    }
}

void SimulationWidget::showDingquePanel() {
    m_dingquePanel->show();
}

void SimulationWidget::hideDingquePanel() {
    m_dingquePanel->hide();
}

QString SimulationWidget::tileToDisplay(Tile t) const {
    return t.toString();
}

QString SimulationWidget::suitColor(TileSuit suit) const {
    switch (suit) {
        case TileSuit::Dot: return "#cc4444";      // 红色 筒
        case TileSuit::Bamboo: return "#44aa44";    // 绿色 条
        case TileSuit::Character: return "#4488cc"; // 蓝色 万
    }
    return "#888888";
}

bool SimulationWidget::isHumanTurn() const {
    return m_engine->currentPlayer() == 0
        && !m_engine->playerState(0).hasWon
        && m_engine->phase() == GameEngine::Playing;
}

void SimulationWidget::runAITurn() {
    if (m_engine->phase() != GameEngine::Playing) return;

    int cp = m_engine->currentPlayer();
    int lp = m_engine->lastDiscardPlayer();

    // 出牌响应阶段：有人刚出牌，按优先级(胡>杠>碰)检查 AI 响应
    if (lp >= 0) {
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;

            if (m_engine->canWinOnDiscard(i) && m_ai->shouldWin()) {
                m_engine->playerWinOnDiscard(i);
                refreshAllPlayerUI();
                return;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;

            if (m_engine->canExposedKong(i) &&
                m_ai->shouldKong(m_engine->lastDiscard(),
                                 m_engine->playerState(i).hand,
                                 m_engine->playerState(i).dingque)) {
                m_engine->playerKong(i);
                refreshAllPlayerUI();
                return;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;

            if (m_engine->canPong(i) &&
                m_ai->shouldPong(m_engine->lastDiscard(),
                                 m_engine->playerState(i).hand,
                                 m_engine->playerState(i).dingque)) {
                m_engine->playerPong(i);
                refreshAllPlayerUI();
                return;
            }
        }
        // 人类玩家可响应时等待其输入，否则自动推进
        if (m_engine->canWinOnDiscard(0) ||
            m_engine->canExposedKong(0) ||
            m_engine->canPong(0)) {
            return;
        }
        m_engine->advanceTurn();
        refreshAllPlayerUI();
        return;
    }

    // 正常回合：当前 AI 玩家摸牌后出牌
    if (cp == 0 || m_engine->playerState(cp).isHuman) return;
    if (m_engine->playerState(cp).hasWon) return;

    GameContext ctx;
    for (int i = 0; i < 4; ++i) {
        ctx.discards[i] = m_engine->playerState(i).discards;
        ctx.melds[i] = m_engine->playerState(i).melds;
    }
    ctx.aliveCount = m_engine->aliveCount();

    Tile discard = m_ai->chooseDiscard(
        m_engine->playerState(cp).hand,
        m_engine->playerState(cp).dingque,
        ctx
    );

    m_engine->playerDiscard(discard);
    refreshAllPlayerUI();
}
