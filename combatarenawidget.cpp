#include "combatarenawidget.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSplitter>
#include <QStandardPaths>
#include <QProcess>
#include <QSvgRenderer>
#include <QPainter>
#include <QBuffer>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <algorithm>

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

static QString tileToHtml(Tile tile, int width = 22, int height = 32) {
    QPixmap pix = tileIcon(tile);
    if (pix.isNull()) {
        return tile.toString();
    }
    QImage img = pix.toImage().scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    QString base64 = QString::fromLatin1(data.toBase64());
    return QString("<img src='data:image/png;base64,%1' width='%2' height='%3'/>")
        .arg(base64)
        .arg(width)
        .arg(height);
}

// ===== 构造 =====

CombatArenaWidget::CombatArenaWidget(ArenaMode mode, QWidget *parent)
    : QWidget(parent), m_mode(mode)
{
    m_engine = new GameEngine(this);
    m_ai = new AIPlayer(this);
    m_aiTimer = new QTimer(this);
    m_aiTimer->setSingleShot(true);
    m_aiTimer->setInterval(700);
    connect(m_aiTimer, &QTimer::timeout, this, &CombatArenaWidget::onAITimerTick);

    setupUI();

    // 连接引擎信号
    connect(m_engine, &GameEngine::gameStarted,
            this, &CombatArenaWidget::onGameStarted);
    connect(m_engine, &GameEngine::turnChanged,
            this, &CombatArenaWidget::onTurnChanged);
    connect(m_engine, &GameEngine::playerDiscarded,
            this, &CombatArenaWidget::onPlayerDiscarded);
    connect(m_engine, &GameEngine::playerPonged,
            this, &CombatArenaWidget::onPlayerPonged);
    connect(m_engine, &GameEngine::playerKonged,
            this, &CombatArenaWidget::onPlayerKonged);
    connect(m_engine, &GameEngine::playerWon,
            this, &CombatArenaWidget::onPlayerWon);
    connect(m_engine, &GameEngine::dingqueSet,
            this, &CombatArenaWidget::onDingqueSet);
    connect(m_engine, &GameEngine::gameOver,
            this, &CombatArenaWidget::onGameOver);
    connect(m_engine, &GameEngine::logMessage,
            this, &CombatArenaWidget::onLogMessage);
}

// ===== UI 构建 =====

void CombatArenaWidget::setupUI() {
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(4, 4, 4, 4);
    outerLayout->setSpacing(10);
    setMinimumSize(960, 700);

    // 标题栏
    QHBoxLayout *titleBar = new QHBoxLayout();

    QPushButton *backBtn = new QPushButton("← 返回", this);
    backBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #aaa; font-size: 13px; "
        "padding: 4px 8px; border: 1px solid #555; border-radius: 4px; }"
        "QPushButton:hover { color: #fff; border-color: #888; }");
    connect(backBtn, &QPushButton::clicked, this, &CombatArenaWidget::backRequested);
    titleBar->addWidget(backBtn);

    QLabel *title = new QLabel(
        m_mode == ArenaMode::RealtimeAssist
            ? "<b>实时辅助决策</b>"
            : "<b>AI 模拟训练</b>",
        this);
    title->setStyleSheet("font-size: 16px;");
    titleBar->addWidget(title);
    titleBar->addStretch();

    QPushButton *newGameBtn = new QPushButton("新游戏", this);
    newGameBtn->setStyleSheet(
        "QPushButton { background-color: #D77757; color: white; font-size: 14px; "
        "padding: 8px 16px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #c06648; }");
    connect(newGameBtn, &QPushButton::clicked, this, &CombatArenaWidget::onNewGame);
    titleBar->addWidget(newGameBtn);

    m_wallLabel = new QLabel("牌墙: --", this);
    titleBar->addWidget(m_wallLabel);
    outerLayout->addLayout(titleBar);

    // 抓包栏 (仅实时模式)
    if (m_mode == ArenaMode::RealtimeAssist) {
        m_captureBar = new QWidget(this);
        QHBoxLayout *captureLayout = new QHBoxLayout(m_captureBar);
        captureLayout->setContentsMargins(0, 2, 0, 2);
        QLabel *captureLabel = new QLabel("抓包方式:", this);
        captureLayout->addWidget(captureLabel);

        m_captureCombo = new QComboBox(this);
        m_captureCombo->addItems({"视频抓包", "语音抓包", "手动抓包"});
        captureLayout->addWidget(m_captureCombo);

        m_captureBtn = new QPushButton("开始抓包", this);
        m_captureBtn->setStyleSheet(
            "QPushButton { background-color: #5769F7; color: white; padding: 4px 12px; "
            "border-radius: 3px; }");
        connect(m_captureBtn, &QPushButton::clicked,
                this, &CombatArenaWidget::onManualCapture);
        captureLayout->addWidget(m_captureBtn);

        // 更新牌局按钮 (注入牌局后使用)
        m_updateBtn = new QPushButton("更新牌局", this);
        m_updateBtn->setStyleSheet(
            "QPushButton { background-color: #D77757; color: white; padding: 4px 12px; "
            "border-radius: 3px; }"
            "QPushButton:hover { background-color: #c06648; }");
        connect(m_updateBtn, &QPushButton::clicked,
                this, &CombatArenaWidget::onUpdateGameState);
        captureLayout->addWidget(m_updateBtn);

        captureLayout->addStretch();
        outerLayout->addWidget(m_captureBar);
    } else {
        m_captureBar = nullptr;
        m_captureCombo = nullptr;
        m_captureBtn = nullptr;
        m_updateBtn = nullptr;
    }

    // 视频抓包区域 (仅实时模式, 默认隐藏)
    if (m_mode == ArenaMode::RealtimeAssist) {
        m_videoCapture = new VideoCaptureWidget(this);
        m_videoCapture->setVisible(false);
        outerLayout->addWidget(m_videoCapture);
    } else {
        m_videoCapture = nullptr;
    }

    // 语音抓包面板 (仅实时模式, 默认隐藏)
    if (m_mode == ArenaMode::RealtimeAssist) {
        m_voicePanel = new QWidget(this);
        QHBoxLayout *voiceLayout = new QHBoxLayout(m_voicePanel);
        voiceLayout->setContentsMargins(0, 2, 0, 2);

        QLabel *voiceHint = new QLabel("语音指令 (双击Fn键启用听写):", m_voicePanel);
        voiceHint->setStyleSheet("color: #aaa; font-size: 12px;");
        voiceLayout->addWidget(voiceHint);

        m_voiceInput = new QLineEdit(m_voicePanel);
        m_voiceInput->setPlaceholderText("输入或说出指令，如: 摸3筒 打5万 碰8条 胡");
        m_voiceInput->setStyleSheet(
            "QLineEdit { background-color: #2a2a2a; color: #ddd; border: 1px solid #555; "
            "border-radius: 4px; padding: 6px 10px; font-size: 14px; }");
        connect(m_voiceInput, &QLineEdit::returnPressed,
                this, &CombatArenaWidget::onVoiceCommandSubmit);
        voiceLayout->addWidget(m_voiceInput, 1);

        m_voiceSubmitBtn = new QPushButton("提交", m_voicePanel);
        m_voiceSubmitBtn->setStyleSheet(
            "QPushButton { background-color: #D77757; color: white; padding: 6px 16px; "
            "border-radius: 4px; font-size: 13px; }"
            "QPushButton:hover { background-color: #c06648; }");
        connect(m_voiceSubmitBtn, &QPushButton::clicked,
                this, &CombatArenaWidget::onVoiceCommandSubmit);
        voiceLayout->addWidget(m_voiceSubmitBtn);

        m_voicePanel->setVisible(false);
        outerLayout->addWidget(m_voicePanel);
    } else {
        m_voicePanel = nullptr;
        m_voiceInput = nullptr;
        m_voiceSubmitBtn = nullptr;
    }

    // 主分割器 (左2/3 游戏面板, 右1/3 侧边栏)
    m_splitter = new QSplitter(Qt::Horizontal, this);

    // -- 左侧: 游戏面板 --
    m_gamePanel = new QWidget(this);
    QVBoxLayout *gameLayout = new QVBoxLayout(m_gamePanel);
    gameLayout->setContentsMargins(0, 0, 4, 0);

    // 对手区域 (3x3 网格)
    QGridLayout *oppGrid = new QGridLayout();
    setupOpponentAreas(oppGrid);
    gameLayout->addLayout(oppGrid);

    // 自家面子展示区 (碰/杠的牌组)
    m_selfMeldLabel = new QLabel("", this);
    m_selfMeldLabel->setAlignment(Qt::AlignCenter);
    m_selfMeldLabel->setTextFormat(Qt::RichText);
    m_selfMeldLabel->setStyleSheet(
        "font-size: 16px; color: #333; padding: 4px; "
        "background-color: #e8e8e8; border-radius: 4px; "
        "min-height: 30px;");
    gameLayout->addWidget(m_selfMeldLabel);

    gameLayout->addStretch();

    // 中央信息区
    QHBoxLayout *centerBar = new QHBoxLayout();

    m_lastDiscardLabel = new QLabel("最后打出:<br>---", this);
    m_lastDiscardLabel->setAlignment(Qt::AlignCenter);
    m_lastDiscardLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    m_lastDiscardLabel->setTextFormat(Qt::RichText);
    centerBar->addWidget(m_lastDiscardLabel);

    centerBar->addStretch();

    m_statusLabel = new QLabel("点击「新游戏」开始", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 14px; color: #D77757;");
    centerBar->addWidget(m_statusLabel);

    centerBar->addStretch();

    // 操作按钮区
    m_actionPanel = new QWidget(this);
    QHBoxLayout *actionLayout = new QHBoxLayout(m_actionPanel);

    m_winBtn = new QPushButton("胡!", this);
    m_winBtn->setStyleSheet(
        "QPushButton { background-color: #cc4444; color: white; font-size: 14px; "
        "padding: 8px 14px; font-weight: bold; border-radius: 4px; }");
    connect(m_winBtn, &QPushButton::clicked, this, &CombatArenaWidget::onWinClicked);

    m_kongBtn = new QPushButton("杠", this);
    m_kongBtn->setStyleSheet(
        "QPushButton { background-color: #8888ff; color: white; font-size: 14px; "
        "padding: 8px 14px; border-radius: 4px; }");
    connect(m_kongBtn, &QPushButton::clicked, this, &CombatArenaWidget::onKongClicked);

    m_pongBtn = new QPushButton("碰", this);
    m_pongBtn->setStyleSheet(
        "QPushButton { background-color: #ddaa00; color: white; font-size: 14px; "
        "padding: 8px 14px; border-radius: 4px; }");
    connect(m_pongBtn, &QPushButton::clicked, this, &CombatArenaWidget::onPongClicked);

    m_selfKongBtn = new QPushButton("暗杠/加杠", this);
    m_selfKongBtn->setStyleSheet(
        "QPushButton { background-color: #6666cc; color: white; font-size: 14px; "
        "padding: 8px 14px; border-radius: 4px; }");
    connect(m_selfKongBtn, &QPushButton::clicked,
            this, &CombatArenaWidget::onSelfKongClicked);

    m_passBtn = new QPushButton("过", this);
    m_passBtn->setStyleSheet(
        "QPushButton { background-color: #888888; color: white; font-size: 14px; "
        "padding: 8px 14px; border-radius: 4px; }");
    connect(m_passBtn, &QPushButton::clicked, this, &CombatArenaWidget::onPassClicked);

    actionLayout->addWidget(m_winBtn);
    actionLayout->addWidget(m_kongBtn);
    actionLayout->addWidget(m_pongBtn);
    actionLayout->addWidget(m_selfKongBtn);
    actionLayout->addWidget(m_passBtn);
    m_actionPanel->hide();
    centerBar->addWidget(m_actionPanel);

    // 定缺面板
    m_dingquePanel = new QWidget(this);
    QHBoxLayout *dingqueLayout = new QHBoxLayout(m_dingquePanel);
    QPushButton *dqDotBtn = new QPushButton("缺筒", this);
    dqDotBtn->setStyleSheet(
        "background-color: #cc4444; color: white; font-size: 14px; padding: 10px 18px; "
        "border-radius: 4px;");
    QPushButton *dqBamBtn = new QPushButton("缺条", this);
    dqBamBtn->setStyleSheet(
        "background-color: #44aa44; color: white; font-size: 14px; padding: 10px 18px; "
        "border-radius: 4px;");
    QPushButton *dqCharBtn = new QPushButton("缺万", this);
    dqCharBtn->setStyleSheet(
        "background-color: #4444cc; color: white; font-size: 14px; padding: 10px 18px; "
        "border-radius: 4px;");
    connect(dqDotBtn, &QPushButton::clicked,
            this, [this]() { onDingqueClicked(TileSuit::Dot); });
    connect(dqBamBtn, &QPushButton::clicked,
            this, [this]() { onDingqueClicked(TileSuit::Bamboo); });
    connect(dqCharBtn, &QPushButton::clicked,
            this, [this]() { onDingqueClicked(TileSuit::Character); });
    dingqueLayout->addWidget(dqDotBtn);
    dingqueLayout->addWidget(dqBamBtn);
    dingqueLayout->addWidget(dqCharBtn);
    dingqueLayout->addStretch();
    m_dingquePanel->hide();
    centerBar->addWidget(m_dingquePanel);

    gameLayout->addLayout(centerBar);

    // 手牌区域
    QScrollArea *handScroll = new QScrollArea(this);
    handScroll->setWidgetResizable(true);
    handScroll->setMinimumHeight(100);
    handScroll->setMaximumHeight(120);
    handScroll->setStyleSheet("QScrollArea { border: 1px solid #555; border-radius: 4px; background: #f8f8f8; }");
    QWidget *handContainer = new QWidget(this);
    m_handLayout = new QHBoxLayout(handContainer);
    m_handLayout->setSpacing(2);
    m_handLayout->setContentsMargins(4, 4, 4, 4);
    m_handLayout->addStretch();
    handScroll->setWidget(handContainer);
    gameLayout->addWidget(handScroll);

    // 日志
    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(80);
    m_logView->setPlaceholderText("游戏日志...");
    gameLayout->addWidget(m_logView);

    m_splitter->addWidget(m_gamePanel);

    // -- 右侧: 侧边栏 --
    m_sidebar = new QWidget(this);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(4, 0, 0, 0);
    setupSidebar(sidebarLayout);
    m_splitter->addWidget(m_sidebar);

    // 设置分割比例 2:1
    m_splitter->setStretchFactor(0, 2);
    m_splitter->setStretchFactor(1, 1);

    outerLayout->addWidget(m_splitter);
}

void CombatArenaWidget::setupOpponentAreas(QGridLayout *grid) {
    grid->setSpacing(16);
    grid->setContentsMargins(8, 4, 8, 4);

    // 三列均分宽度
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);

    // 对家 (player 2) — 顶部中间
    m_opponentBoxes[2] = createOpponentBox(2);
    grid->addWidget(m_opponentBoxes[2], 0, 1, Qt::AlignHCenter);

    // 占位列: row 0 左右留空
    grid->addItem(new QSpacerItem(100, 1), 0, 0);
    grid->addItem(new QSpacerItem(100, 1), 0, 2);

    // 上家 (player 3) — 左边
    m_opponentBoxes[3] = createOpponentBox(3);
    grid->addWidget(m_opponentBoxes[3], 1, 0, Qt::AlignLeft);

    // 下家 (player 1) — 右边
    m_opponentBoxes[1] = createOpponentBox(1);
    grid->addWidget(m_opponentBoxes[1], 1, 2, Qt::AlignRight);

    // 中央占位
    QLabel *selfPlaceholder = new QLabel("", this);
    selfPlaceholder->setMinimumHeight(40);
    grid->addWidget(selfPlaceholder, 2, 1, Qt::AlignCenter);

    // 行高
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);
    grid->setRowStretch(2, 0);
    grid->setRowMinimumHeight(0, 120);
    grid->setRowMinimumHeight(1, 120);
}

QGroupBox *CombatArenaWidget::createOpponentBox(int seatIndex) {
    int player = seatIndex;
    QString name;
    switch (seatIndex) {
        case 1: name = "下家 (玩家2)"; break;
        case 2: name = "对家 (玩家3)"; break;
        case 3: name = "上家 (玩家4)"; break;
        default: name = "自己";
    }

    QGroupBox *box = new QGroupBox("", this);
    box->setMinimumWidth(210);
    box->setMaximumWidth(300);
    box->setStyleSheet(
        "QGroupBox { background-color: #f5f5f5; border: 1px solid #ccc; "
        "border-radius: 6px; padding: 8px 10px 8px 10px; }");
    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->setSpacing(8);
    layout->setContentsMargins(6, 0, 6, 6);

    // 用 QLabel 代替 QGroupBox 原生标题，彻底避免重叠问题
    QLabel *titleLabel = new QLabel(name, this);
    titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 13px; color: #222; "
        "padding: 2px 0;");
    layout->addWidget(titleLabel);

    m_oppDingqueLabels[player] = new QLabel("定缺: --", this);
    m_oppDingqueLabels[player]->setStyleSheet("font-size: 12px; color: #555;");
    layout->addWidget(m_oppDingqueLabels[player]);

    m_oppMeldLabels[player] = new QLabel("", this);
    m_oppMeldLabels[player]->setWordWrap(true);
    m_oppMeldLabels[player]->setTextFormat(Qt::RichText);
    m_oppMeldLabels[player]->setStyleSheet("font-size: 12px; color: #333;");
    layout->addWidget(m_oppMeldLabels[player]);

    m_oppDiscardLabels[player] = new QLabel("弃牌: ", this);
    m_oppDiscardLabels[player]->setWordWrap(true);
    m_oppDiscardLabels[player]->setTextFormat(Qt::RichText);
    m_oppDiscardLabels[player]->setMinimumHeight(42);
    m_oppDiscardLabels[player]->setStyleSheet("font-size: 12px; color: #444; padding: 2px;");
    layout->addWidget(m_oppDiscardLabels[player]);

    m_oppPeekButtons[player] = new QPushButton("查看手牌", this);
    m_oppPeekButtons[player]->setStyleSheet(
        "QPushButton { font-size: 12px; padding: 5px 12px; "
        "background-color: #e0e0e0; color: #333; border-radius: 3px; border: 1px solid #bbb; }"
        "QPushButton:hover { background-color: #d0d0d0; color: #111; }"
        "QPushButton:disabled { background-color: #f0f0f0; color: #aaa; border-color: #ddd; }");
    connect(m_oppPeekButtons[player], &QPushButton::clicked, this, [this, player]() {
        onPeekOpponent(player);
    });
    layout->addWidget(m_oppPeekButtons[player]);

    return box;
}

void CombatArenaWidget::setupSidebar(QVBoxLayout *sidebarLayout) {
    // 出牌建议
    m_suggestGroup = new QGroupBox("出牌建议", this);
    m_suggestGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 13px; border: 1px solid #D77757; "
        "border-radius: 4px; margin-top: 8px; padding-top: 12px; }"
        "QGroupBox::title { color: #D77757; }");
    QVBoxLayout *suggestLayout = new QVBoxLayout(m_suggestGroup);
    m_suggestionBox = new QTextEdit(this);
    m_suggestionBox->setReadOnly(true);
    m_suggestionBox->setMinimumHeight(180);
    m_suggestionBox->setPlaceholderText("等待游戏开始...");
    suggestLayout->addWidget(m_suggestionBox);
    sidebarLayout->addWidget(m_suggestGroup);

    // 对手画像
    m_profileGroup = new QGroupBox("对手画像", this);
    m_profileGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 13px; border: 1px solid #5769F7; "
        "border-radius: 4px; margin-top: 8px; padding-top: 12px; }"
        "QGroupBox::title { color: #5769F7; }");
    QVBoxLayout *profileLayout = new QVBoxLayout(m_profileGroup);

    QString seatNames[3] = {"上家", "下家", "对家"};
    for (int i = 0; i < 3; ++i) {
        QGroupBox *seatBox = new QGroupBox(seatNames[i], this);
        QVBoxLayout *seatLayout = new QVBoxLayout(seatBox);

        m_profileStyleLabel[i] = new QLabel("风格: 等待数据...", this);
        seatLayout->addWidget(m_profileStyleLabel[i]);

        m_profileDangerLabel[i] = new QLabel("高危炮牌: --", this);
        m_profileDangerLabel[i]->setWordWrap(true);
        seatLayout->addWidget(m_profileDangerLabel[i]);

        profileLayout->addWidget(seatBox);
    }
    sidebarLayout->addWidget(m_profileGroup);
}

// ===== 座位映射 =====

int CombatArenaWidget::mapSeatToPlayer(int seatIndex) const {
    return seatIndex; // 直接映射
}

int CombatArenaWidget::mapPlayerToSeat(int player) const {
    return player;
}

// ===== 游戏控制 =====

void CombatArenaWidget::onNewGame() {
    if (m_mode == ArenaMode::RealtimeAssist) {
        m_logView->append("[实战模式] 请先选择抓包方式并输入牌局数据，而非自动开局");
        return;
    }
    m_engine->startNewGame();
}

void CombatArenaWidget::onDingqueClicked(TileSuit suit) {
    m_engine->setDingque(0, suit);
    hideDingquePanel();
}

void CombatArenaWidget::onHandTileClicked(int index) {
    if (!isHumanTurn()) return;
    if (m_engine->phase() != GameEngine::Playing) return;
    if (index < 0 || index >= static_cast<int>(m_displayHand.size())) return;

    Tile tile = m_displayHand[index];

    // 定缺规则提示: 缺门未打完时不能打其他花色
    const PlayerState &ps = m_engine->playerState(0);
    if (ps.dingqueSet && !ps.dingqueDone && tile.suit != ps.dingque) {
        QString suitName = ps.dingque == TileSuit::Dot ? "筒" : (ps.dingque == TileSuit::Bamboo ? "条" : "万");
        m_logView->append(QString("<font color='orange'>定缺规则: 缺门(%1)未打完，必须优先打出%1!</font>").arg(suitName));
        return;
    }

    m_engine->playerDiscard(tile);
}

void CombatArenaWidget::onPongClicked() {
    m_engine->playerPong(0);
    hideDingquePanel();
    refreshAllUI();
}

void CombatArenaWidget::onKongClicked() {
    m_engine->playerKong(0);
    hideDingquePanel();
    refreshAllUI();
}

void CombatArenaWidget::onSelfKongClicked() {
    PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
    const auto &hand = ps.hand;

    // 优先检查暗杠：手牌中有4张相同的
    std::vector<int> counts = MahjongLogic::tilesToCounts(hand);
    for (int i = 0; i < 30; ++i) {
        if (counts[i] >= 4) {
            Tile tile = {static_cast<TileSuit>(i / 10), i % 10};
            // 暗杠：手牌中移除4张，加入面子
            std::vector<Tile> kongTiles;
            int removed = 0;
            auto it = ps.hand.begin();
            while (it != ps.hand.end() && removed < 4) {
                if (*it == tile) {
                    kongTiles.push_back(*it);
                    it = ps.hand.erase(it);
                    removed++;
                } else {
                    ++it;
                }
            }
            if (removed >= 4) {
                ps.melds.push_back(kongTiles);
                // 杠后补牌
                if (m_engine->tilesRemaining() > 0) {
                    // 手动模拟补牌：从牌墙摸一张
                    // 用引擎现有方法处理
                }
                m_logView->append(QString("[暗杠] 暗杠了 %1").arg(tile.toString()));
                // 杠后仍然是自己的回合
                m_manualTurnPlayer = 0;
                m_statusLabel->setText("你的回合 (暗杠后)");
                hideDingquePanel();
                refreshAllUI();
                return;
            }
        }
    }

    // 检查加杠：已碰的牌中，手牌有第4张
    for (auto &meld : ps.melds) {
        if (meld.size() == 3) {
            Tile meldTile = meld[0];
            for (auto it = ps.hand.begin(); it != ps.hand.end(); ++it) {
                if (*it == meldTile) {
                    meld.push_back(*it);
                    ps.hand.erase(it);
                    m_logView->append(QString("[加杠] 加杠了 %1").arg(meldTile.toString()));
                    m_manualTurnPlayer = 0;
                    m_statusLabel->setText("你的回合 (加杠后)");
                    hideDingquePanel();
                    refreshAllUI();
                    return;
                }
            }
        }
    }
}

void CombatArenaWidget::onWinClicked() {
    if (m_mode == ArenaMode::RealtimeAssist) {
        // 实时模式下直接操作 PlayerState，绕过引擎的 m_currentPlayer 检查
        PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
        bool canSW = (!ps.dingqueSet || ps.dingqueDone) && MahjongLogic::canWin(ps.hand);
        if (canSW) {
            ps.hasWon = true;
            m_logView->append("[自摸] 你胡了!");
            m_statusLabel->setText("你胡了! 自摸");
        } else if (m_engine->canWinOnDiscard(0)) {
            m_engine->playerWinOnDiscard(0);
        }
    } else {
        if (m_engine->canSelfWin(0)) {
            m_engine->playerSelfWin(0);
        } else if (m_engine->canWinOnDiscard(0)) {
            m_engine->playerWinOnDiscard(0);
        }
    }
    hideDingquePanel();
    refreshAllUI();
}

void CombatArenaWidget::onPassClicked() {
    m_engine->advanceTurn();
    hideDingquePanel();
    refreshAllUI();
}

void CombatArenaWidget::onManualCapture() {
    QString method = m_captureCombo ? m_captureCombo->currentText() : "手动抓包";
    m_logView->append(QString("[抓包] 使用 %1 方式获取牌局信息...").arg(method));

    if (method == "视频抓包") {
        onStartVideoCapture();
        return;
    }

    if (method == "语音抓包") {
        onStartVoiceCapture();
        return;
    }

    if (method != "手动抓包") {
        m_logView->append(QString("[抓包] %1 已启动 (功能待实现)").arg(method));
        return;
    }

    // ===== 手动抓包对话框 =====
    QDialog dlg(this);
    dlg.setWindowTitle("手动输入牌局");
    dlg.setMinimumWidth(500);
    QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);
    dlgLayout->setSpacing(10);

    // --- 手牌输入 ---
    dlgLayout->addWidget(new QLabel("<b>你的手牌 (13或14张):</b>", &dlg));
    QTextEdit *handInput = new QTextEdit(&dlg);
    handInput->setPlaceholderText("例: 1筒 1筒 2筒 3筒 4筒 5筒 6筒 7筒 8筒 8筒 9筒 9筒 9筒");
    handInput->setMaximumHeight(60);
    dlgLayout->addWidget(handInput);

    // --- 定缺 ---
    dlgLayout->addWidget(new QLabel("<b>你的定缺:</b>", &dlg));
    QHBoxLayout *dqLayout = new QHBoxLayout();
    QPushButton *dqDot = new QPushButton("缺筒", &dlg);
    QPushButton *dqBam = new QPushButton("缺条", &dlg);
    QPushButton *dqChar = new QPushButton("缺万", &dlg);
    dqDot->setStyleSheet("background-color: #cc4444; color: white; padding: 6px 14px; border-radius: 3px;");
    dqBam->setStyleSheet("background-color: #44aa44; color: white; padding: 6px 14px; border-radius: 3px;");
    dqChar->setStyleSheet("background-color: #4444cc; color: white; padding: 6px 14px; border-radius: 3px;");
    dqLayout->addWidget(dqDot);
    dqLayout->addWidget(dqBam);
    dqLayout->addWidget(dqChar);
    dqLayout->addStretch();
    dlgLayout->addLayout(dqLayout);

    // 用 lambda 捕获定缺选择
    TileSuit *selectedDingque = new TileSuit(TileSuit::Dot);
    QObject::connect(dqDot, &QPushButton::clicked, [selectedDingque, dqDot, dqBam, dqChar]() {
        *selectedDingque = TileSuit::Dot;
        dqDot->setText("缺筒 ✓"); dqBam->setText("缺条"); dqChar->setText("缺万");
    });
    QObject::connect(dqBam, &QPushButton::clicked, [selectedDingque, dqDot, dqBam, dqChar]() {
        *selectedDingque = TileSuit::Bamboo;
        dqDot->setText("缺筒"); dqBam->setText("缺条 ✓"); dqChar->setText("缺万");
    });
    QObject::connect(dqChar, &QPushButton::clicked, [selectedDingque, dqDot, dqBam, dqChar]() {
        *selectedDingque = TileSuit::Character;
        dqDot->setText("缺筒"); dqBam->setText("缺条"); dqChar->setText("缺万 ✓");
    });

    // --- 对手弃牌 (可选) ---
    dlgLayout->addWidget(new QLabel("<b>已观察到的弃牌 (选填):</b>", &dlg));
    QGridLayout *oppGrid = new QGridLayout();
    oppGrid->addWidget(new QLabel("下家弃牌:", &dlg), 0, 0);
    QLineEdit *discard1 = new QLineEdit(&dlg);
    discard1->setPlaceholderText("如: 3万 7条");
    oppGrid->addWidget(discard1, 0, 1);

    oppGrid->addWidget(new QLabel("对家弃牌:", &dlg), 1, 0);
    QLineEdit *discard2 = new QLineEdit(&dlg);
    discard2->setPlaceholderText("如: 1筒 5万");
    oppGrid->addWidget(discard2, 1, 1);

    oppGrid->addWidget(new QLabel("上家弃牌:", &dlg), 2, 0);
    QLineEdit *discard3 = new QLineEdit(&dlg);
    discard3->setPlaceholderText("如: 8条 9筒");
    oppGrid->addWidget(discard3, 2, 1);
    dlgLayout->addLayout(oppGrid);

    // --- 按钮 ---
    QDialogButtonBox *btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    btns->button(QDialogButtonBox::Ok)->setText("确认开局");
    QObject::connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    dlgLayout->addWidget(btns);

    if (dlg.exec() != QDialog::Accepted) {
        delete selectedDingque;
        return;
    }

    // --- 解析输入 ---
    std::vector<Tile> hand = MahjongLogic::parseTileList(handInput->toPlainText());
    if (hand.size() < 13) {
        m_logView->append(QString("[错误] 手牌不足13张 (输入了%1张)").arg(hand.size()));
        delete selectedDingque;
        return;
    }
    if (hand.size() > 14) {
        hand.resize(14);
    }

    TileSuit dingque = *selectedDingque;
    delete selectedDingque;

    m_logView->append(QString("[抓包] 解析手牌 %1 张, 定缺: %2")
        .arg(hand.size())
        .arg(dingque == TileSuit::Dot ? "筒" : (dingque == TileSuit::Bamboo ? "条" : "万")));

    // --- 注入引擎 ---
    m_engine->injectRealtimeGame(hand, dingque);

    // 解析对手弃牌
    QLineEdit *discardFields[3] = {discard1, discard2, discard3};
    int oppPlayers[3] = {1, 2, 3}; // 下家=1, 对家=2, 上家=3
    for (int i = 0; i < 3; ++i) {
        std::vector<Tile> discs = MahjongLogic::parseTileList(discardFields[i]->text());
        if (!discs.empty()) {
            PlayerState &ops = const_cast<PlayerState &>(m_engine->playerState(oppPlayers[i]));
            ops.discards = discs;
            m_logView->append(QString("[抓包] 玩家%1 弃牌 %2 张")
                .arg(oppPlayers[i] + 1).arg(discs.size()));
        }
    }

    hideDingquePanel();
    m_statusLabel->setText(QString("牌局已注入 | 定缺: %1 | 点击手牌打出或使用「更新牌局」")
        .arg(dingque == TileSuit::Dot ? "缺筒" : (dingque == TileSuit::Bamboo ? "缺条" : "缺万")));
    refreshAllUI();
    refreshSuggestions();
    refreshOpponentProfiles();
    m_logView->append("[抓包] 牌局注入完成，可开始使用「更新牌局」同步每轮变化");
    m_manualTurnPlayer = 0;
    m_statusLabel->setText("当前回合: 你 | 更新牌局同步变化");
}

void CombatArenaWidget::onUpdateGameState() {
    if (m_engine->phase() == GameEngine::Idle) {
        m_logView->append("[更新] 请先通过「开始抓包」注入牌局!");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("更新牌局");
    dlg.setMinimumWidth(450);
    QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);
    dlgLayout->setSpacing(8);

    dlgLayout->addWidget(new QLabel("<b>本轮变化:</b>", &dlg));

    // 你摸到的牌
    QHBoxLayout *drawRow = new QHBoxLayout();
    drawRow->addWidget(new QLabel("你摸到:", &dlg));
    QLineEdit *drawnInput = new QLineEdit(&dlg);
    drawnInput->setPlaceholderText("如: 7筒 (没有则留空)");
    drawRow->addWidget(drawnInput);
    dlgLayout->addLayout(drawRow);

    // 你打出的牌
    QHBoxLayout *discRow = new QHBoxLayout();
    discRow->addWidget(new QLabel("你打出:", &dlg));
    QLineEdit *myDiscardInput = new QLineEdit(&dlg);
    myDiscardInput->setPlaceholderText("如: 3万 (没有则留空)");
    discRow->addWidget(myDiscardInput);
    dlgLayout->addLayout(discRow);

    // --- 动态事件列表 ---
    dlgLayout->addWidget(new QLabel("<b>牌桌事件:</b>", &dlg));

    QVBoxLayout *eventList = new QVBoxLayout();
    eventList->setSpacing(4);

    struct EventRow {
        QWidget *container;
        QComboBox *player;
        QComboBox *action;
        QLineEdit *tile;
        QLineEdit *handCount;
    };
    std::vector<EventRow> eventRows;

    auto addEventRow = [&](bool canRemove) {
        EventRow row;
        row.container = new QWidget(&dlg);
        QHBoxLayout *r = new QHBoxLayout(row.container);
        r->setContentsMargins(0, 2, 0, 2);

        r->addWidget(new QLabel("玩家:", &dlg));
        row.player = new QComboBox(&dlg);
        row.player->addItems({"下家", "对家", "上家"});
        r->addWidget(row.player);

        row.action = new QComboBox(&dlg);
        row.action->addItems({"打出", "碰", "明杠", "暗杠"});
        r->addWidget(row.action);

        r->addWidget(new QLabel("牌:", &dlg));
        row.tile = new QLineEdit(&dlg);
        row.tile->setPlaceholderText("如: 5万");
        row.tile->setMaximumWidth(60);
        r->addWidget(row.tile);

        r->addWidget(new QLabel("手牌:", &dlg));
        row.handCount = new QLineEdit(&dlg);
        row.handCount->setPlaceholderText("留空");
        row.handCount->setMaximumWidth(45);
        r->addWidget(row.handCount);

        if (canRemove) {
            QPushButton *rmBtn = new QPushButton("✕", &dlg);
            rmBtn->setFixedSize(24, 24);
            rmBtn->setStyleSheet(
                "QPushButton { background: #555; color: #fff; border-radius: 12px; "
                "font-size: 11px; } QPushButton:hover { background: #c44; }");
            int idx = static_cast<int>(eventRows.size());
            connect(rmBtn, &QPushButton::clicked, [&eventRows, idx]() {
                if (idx >= 0 && idx < static_cast<int>(eventRows.size())) {
                    eventRows[idx].container->setVisible(false);
                }
            });
            r->addWidget(rmBtn);
        }

        r->addStretch();
        eventList->addWidget(row.container);
        eventRows.push_back(row);
    };

    addEventRow(false); // first row, not removable

    QPushButton *addBtn = new QPushButton("+ 添加事件", &dlg);
    addBtn->setStyleSheet(
        "QPushButton { background-color: #444; color: #ccc; padding: 4px 12px; "
        "border-radius: 3px; border: 1px dashed #666; }"
        "QPushButton:hover { background-color: #555; }");
    connect(addBtn, &QPushButton::clicked, [&]() { addEventRow(true); });
    eventList->addWidget(addBtn);

    dlgLayout->addLayout(eventList);

    // 回合选择
    QHBoxLayout *turnRow = new QHBoxLayout();
    turnRow->addWidget(new QLabel("接下来谁的回合:", &dlg));
    QComboBox *nextTurnCombo = new QComboBox(&dlg);
    nextTurnCombo->addItems({"我 (玩家1)", "下家 (玩家2)", "对家 (玩家3)", "上家 (玩家4)"});
    nextTurnCombo->setCurrentIndex(m_manualTurnPlayer);
    turnRow->addWidget(nextTurnCombo);
    turnRow->addStretch();
    dlgLayout->addLayout(turnRow);

    QDialogButtonBox *btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    btns->button(QDialogButtonBox::Ok)->setText("确认更新");
    QObject::connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    dlgLayout->addWidget(btns);

    if (dlg.exec() != QDialog::Accepted) return;

    PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));

    // 处理摸牌
    QString drawnStr = drawnInput->text().trimmed();
    if (!drawnStr.isEmpty()) {
        std::vector<Tile> drawn = MahjongLogic::parseTileList(drawnStr);
        if (!drawn.empty()) {
            ps.hand.push_back(drawn[0]);
            std::sort(ps.hand.begin(), ps.hand.end());
            m_logView->append(QString("[更新] 你摸到了 %1").arg(drawn[0].toString()));
        }
    }

    // 处理自己的弃牌
    QString myDiscStr = myDiscardInput->text().trimmed();
    if (!myDiscStr.isEmpty()) {
        std::vector<Tile> discs = MahjongLogic::parseTileList(myDiscStr);
        if (!discs.empty()) {
            MahjongLogic::removeTile(ps.hand, discs[0]);
            ps.discards.push_back(discs[0]);
            m_engine->checkDingqueDone(0);
            m_logView->append(QString("[更新] 你打出了 %1").arg(discs[0].toString()));
        }
    }

    // 处理所有事件行
    for (const auto &row : eventRows) {
        if (!row.container->isVisible()) continue;
        QString tileStr = row.tile->text().trimmed();
        if (tileStr.isEmpty()) continue;

        std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
        if (tiles.empty()) continue;

        Tile t = tiles[0];
        int seat = row.player->currentIndex(); // 0=下家, 1=对家, 2=上家
        int p = (seat == 0) ? 1 : (seat == 1) ? 2 : 3;
        QString action = row.action->currentText();
        PlayerState &ops = const_cast<PlayerState &>(m_engine->playerState(p));

        if (action == "打出") {
            ops.discards.push_back(t);
            m_logView->append(QString("[更新] 玩家%1 打出 %2").arg(p + 1).arg(t.toString()));
        } else if (action == "碰") {
            std::vector<Tile> meld(3, t);
            ops.melds.push_back(meld);
            for (int j = 0; j < 2 && !ops.hand.empty(); ++j) ops.hand.pop_back();
            m_logView->append(QString("[更新] 玩家%1 碰了 %2").arg(p + 1).arg(t.toString()));
        } else if (action == "明杠") {
            std::vector<Tile> meld(4, t);
            ops.melds.push_back(meld);
            for (int j = 0; j < 3 && !ops.hand.empty(); ++j) ops.hand.pop_back();
            m_logView->append(QString("[更新] 玩家%1 杠了 %2 (明杠)").arg(p + 1).arg(t.toString()));
        } else if (action == "暗杠") {
            std::vector<Tile> meld(4, t);
            ops.melds.push_back(meld);
            for (int j = 0; j < 4 && !ops.hand.empty(); ++j) ops.hand.pop_back();
            m_logView->append(QString("[更新] 玩家%1 暗杠了 %2").arg(p + 1).arg(t.toString()));
        }

        // 手牌数手动覆盖
        QString hcStr = row.handCount->text().trimmed();
        if (!hcStr.isEmpty()) {
            bool ok = false;
            int count = hcStr.toInt(&ok);
            if (ok && count >= 0 && count <= 14) {
                while (static_cast<int>(ops.hand.size()) > count && !ops.hand.empty()) {
                    ops.hand.pop_back();
                }
                while (static_cast<int>(ops.hand.size()) < count) {
                    static int dv = 1;
                    ops.hand.push_back({TileSuit::Dot, (dv++ % 9) + 1});
                }
                m_logView->append(QString("[更新] 玩家%1 手牌 → %2 张").arg(p + 1).arg(count));
            }
        }
    }

    // 检查定缺完成
    for (int i = 0; i < 4; ++i) {
        m_engine->checkDingqueDone(i);
    }

    // 更新回合状态
    m_manualTurnPlayer = nextTurnCombo->currentIndex();
    const char *turnNames[] = {"你", "下家", "对家", "上家"};
    QString statusText = QString("当前回合: %1 | 更新牌局同步变化").arg(turnNames[m_manualTurnPlayer]);
    m_statusLabel->setText(
        m_voicePanel && m_voicePanel->isVisible()
            ? statusText + " | 语音面板已启用"
            : (m_videoCapture && m_videoCapture->isVisible()
                ? "视频抓包运行中 | " + statusText
                : statusText));

    refreshAllUI();
    refreshSuggestions();
    refreshOpponentProfiles();
    m_logView->append("[更新] 牌局状态已同步");
}

void CombatArenaWidget::onStartVideoCapture() {
    if (!m_videoCapture) return;

    // 视频抓包配置对话框: 区域 + 定缺 + 初始手牌
    QDialog dlg(this);
    dlg.setWindowTitle("视频抓包 — 配置截取区域与初始牌局");
    dlg.setMinimumWidth(520);
    QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);

    // -- 区域设置 --
    QLabel *regionTitle = new QLabel("截取区域 (麻将游戏窗口在屏幕上的位置):", &dlg);
    regionTitle->setStyleSheet("font-weight: bold; color: #ddd; margin-top: 8px;");
    dlgLayout->addWidget(regionTitle);

    QHBoxLayout *regionRow = new QHBoxLayout();
    QSpinBox *xSpin = new QSpinBox(&dlg);
    xSpin->setRange(0, 3000);
    xSpin->setValue(m_videoCapture->regionX());
    xSpin->setPrefix("X: ");
    regionRow->addWidget(xSpin);

    QSpinBox *ySpin = new QSpinBox(&dlg);
    ySpin->setRange(0, 3000);
    ySpin->setValue(m_videoCapture->regionY());
    ySpin->setPrefix("Y: ");
    regionRow->addWidget(ySpin);

    QSpinBox *wSpin = new QSpinBox(&dlg);
    wSpin->setRange(100, 3000);
    wSpin->setValue(m_videoCapture->regionW());
    wSpin->setPrefix("W: ");
    regionRow->addWidget(wSpin);

    QSpinBox *hSpin = new QSpinBox(&dlg);
    hSpin->setRange(100, 3000);
    hSpin->setValue(m_videoCapture->regionH());
    hSpin->setPrefix("H: ");
    regionRow->addWidget(hSpin);

    QPushButton *testRegionBtn = new QPushButton("测试截屏", &dlg);
    testRegionBtn->setStyleSheet(
        "QPushButton { background-color: #444; color: #ccc; padding: 4px 12px; "
        "border-radius: 3px; } QPushButton:hover { background-color: #555; }");
    regionRow->addWidget(testRegionBtn);
    dlgLayout->addLayout(regionRow);

    // 截图预览标签
    QLabel *testPreview = new QLabel(&dlg);
    testPreview->setMinimumSize(240, 160);
    testPreview->setMaximumHeight(200);
    testPreview->setAlignment(Qt::AlignCenter);
    testPreview->setStyleSheet(
        "QLabel { background-color: #1a1a1a; border: 1px solid #444; "
        "border-radius: 4px; color: #555; }");
    testPreview->setText("点击「测试截屏」预览");
    dlgLayout->addWidget(testPreview);

    // 连接测试截屏
    connect(testRegionBtn, &QPushButton::clicked, [&]() {
        QPixmap pix = VideoCaptureWidget::captureScreen(
            xSpin->value(), ySpin->value(),
            wSpin->value(), hSpin->value());
        if (!pix.isNull()) {
            testPreview->setPixmap(pix.scaled(
                400, 200,
                Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            testPreview->setText(
                "截屏失败\n请授予「屏幕录制」权限:\n"
                "系统设置 → 隐私与安全性 → 屏幕录制");
        }
    });

    // -- 手牌输入 --
    QLabel *handTitle = new QLabel("你的初始手牌 (例如: 1筒 1筒 2筒 ...):", &dlg);
    handTitle->setStyleSheet("font-weight: bold; color: #ddd; margin-top: 12px;");
    dlgLayout->addWidget(handTitle);

    QTextEdit *handInput = new QTextEdit(&dlg);
    handInput->setMaximumHeight(60);
    handInput->setPlaceholderText("例如: 1筒 1筒 2筒 3条 4条 5条 7万 7万 8万 1筒 3筒 5条 6万");
    dlgLayout->addWidget(handInput);

    // -- 定缺选择 --
    QLabel *dqTitle = new QLabel("选择定缺花色:", &dlg);
    dqTitle->setStyleSheet("font-weight: bold; color: #ddd; margin-top: 8px;");
    dlgLayout->addWidget(dqTitle);

    TileSuit *selectedDingque = new TileSuit(TileSuit::Dot);
    QHBoxLayout *dqRow = new QHBoxLayout();

    auto makeDqBtn = [&](const QString &text, TileSuit suit, const QString &color) {
        QPushButton *btn = new QPushButton(text, &dlg);
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; "
                    "padding: 8px 20px; border-radius: 4px; font-size: 14px; }"
                    "QPushButton:hover { opacity: 0.8; }").arg(color));
        connect(btn, &QPushButton::clicked, [&, suit]() { *selectedDingque = suit; });
        dqRow->addWidget(btn);
    };
    makeDqBtn("缺筒", TileSuit::Dot, "#D77757");
    makeDqBtn("缺条", TileSuit::Bamboo, "#57A773");
    makeDqBtn("缺万", TileSuit::Character, "#5769F7");
    dlgLayout->addLayout(dqRow);

    // -- 按钮 --
    QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                    | QDialogButtonBox::Cancel, &dlg);
    btnBox->button(QDialogButtonBox::Ok)->setText("确认开局");
    dlgLayout->addWidget(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) {
        delete selectedDingque;
        return;
    }

    // 解析手牌
    QString handText = handInput->toPlainText().trimmed();
    std::vector<Tile> hand = MahjongLogic::parseTileList(handText);
    if (hand.size() < 13 || hand.size() > 14) {
        m_logView->append(QString("[视频抓包] 手牌数量不正确(%1张), 需要13-14张").arg(hand.size()));
        delete selectedDingque;
        return;
    }

    TileSuit dingque = *selectedDingque;
    delete selectedDingque;

    // 应用区域设置
    m_videoCapture->setRegion(xSpin->value(), ySpin->value(),
                              wSpin->value(), hSpin->value());

    // 显示视频抓包面板并开始截屏
    m_videoCapture->setVisible(true);
    m_videoCapture->startCapture();
    m_statusLabel->setText("视频抓包运行中 | 使用「更新牌局」同步每轮变化");

    // 注入初始牌局
    m_engine->injectRealtimeGame(hand, dingque);

    refreshAllUI();
    refreshSuggestions();
    refreshOpponentProfiles();

    QString dqName = dingque == TileSuit::Dot ? "缺筒"
        : (dingque == TileSuit::Bamboo ? "缺条" : "缺万");
    m_logView->append(QString("[视频抓包] 区域(%1,%2 %3x%4), 手牌%5张, %6 — 截屏已启动")
        .arg(xSpin->value()).arg(ySpin->value())
        .arg(wSpin->value()).arg(hSpin->value())
        .arg(hand.size()).arg(dqName));
}

void CombatArenaWidget::onStartVoiceCapture() {
    // 如果牌局未开始, 先弹出配置对话框
    if (m_engine->phase() == GameEngine::Idle) {
        QDialog dlg(this);
        dlg.setWindowTitle("语音抓包 — 输入初始牌局");
        dlg.setMinimumWidth(500);
        QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);

        QLabel *descLabel = new QLabel(
            "语音抓包使用 macOS 系统听写功能:\n"
            "  1. 双击键盘 Fn 键启动听写\n"
            "  2. 说出指令 (如: 摸3筒, 打5万, 碰8条)\n"
            "  3. 再次按 Fn 键确认输入\n"
            "也可以直接在输入框中打字", &dlg);
        descLabel->setStyleSheet("color: #aaa; font-size: 12px; padding: 8px; "
            "background-color: #2a2a2a; border-radius: 4px;");
        dlgLayout->addWidget(descLabel);

        QLabel *handTitle = new QLabel("你的初始手牌 (例如: 1筒 1筒 2筒 ...):", &dlg);
        handTitle->setStyleSheet("font-weight: bold; color: #ddd; margin-top: 12px;");
        dlgLayout->addWidget(handTitle);

        QTextEdit *handInput = new QTextEdit(&dlg);
        handInput->setMaximumHeight(60);
        handInput->setPlaceholderText("例如: 1筒 1筒 2筒 3条 4条 5条 7万 7万 8万 1筒 3筒 5条 6万");
        dlgLayout->addWidget(handInput);

        QLabel *dqTitle = new QLabel("选择定缺花色:", &dlg);
        dqTitle->setStyleSheet("font-weight: bold; color: #ddd; margin-top: 8px;");
        dlgLayout->addWidget(dqTitle);

        TileSuit *selectedDingque = new TileSuit(TileSuit::Dot);
        QHBoxLayout *dqRow = new QHBoxLayout();
        auto makeDqBtn = [&](const QString &text, TileSuit suit, const QString &color) {
            QPushButton *btn = new QPushButton(text, &dlg);
            btn->setStyleSheet(
                QString("QPushButton { background-color: %1; color: white; "
                        "padding: 8px 20px; border-radius: 4px; font-size: 14px; }")
                    .arg(color));
            connect(btn, &QPushButton::clicked, [&, suit]() { *selectedDingque = suit; });
            dqRow->addWidget(btn);
        };
        makeDqBtn("缺筒", TileSuit::Dot, "#D77757");
        makeDqBtn("缺条", TileSuit::Bamboo, "#57A773");
        makeDqBtn("缺万", TileSuit::Character, "#5769F7");
        dlgLayout->addLayout(dqRow);

        QDialogButtonBox *btnBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        btnBox->button(QDialogButtonBox::Ok)->setText("确认开局");
        dlgLayout->addWidget(btnBox);
        connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) {
            delete selectedDingque;
            return;
        }

        std::vector<Tile> hand = MahjongLogic::parseTileList(handInput->toPlainText().trimmed());
        if (hand.size() < 13 || hand.size() > 14) {
            m_logView->append(QString("[语音抓包] 手牌数量不正确(%1张), 需要13-14张").arg(hand.size()));
            delete selectedDingque;
            return;
        }

        TileSuit dingque = *selectedDingque;
        delete selectedDingque;

        m_engine->injectRealtimeGame(hand, dingque);

        QString dqName = dingque == TileSuit::Dot ? "缺筒"
            : (dingque == TileSuit::Bamboo ? "缺条" : "缺万");
        m_logView->append(QString("[语音抓包] 手牌%1张, %2 — 语音面板已启用")
            .arg(hand.size()).arg(dqName));

        refreshAllUI();
        refreshSuggestions();
        refreshOpponentProfiles();
    }

    // 显示语音面板
    if (m_voicePanel) {
        m_voicePanel->setVisible(true);
        m_voiceInput->setFocus();
    }
    m_statusLabel->setText("当前回合: 你 | 语音面板已启用");
}

void CombatArenaWidget::onVoiceCommandSubmit() {
    if (!m_voiceInput) return;

    QString cmd = m_voiceInput->text().trimmed();
    m_voiceInput->clear();

    if (cmd.isEmpty()) return;

    // 去掉多余空格和标点
    cmd.replace(" ", "");
    cmd.replace("，", "");
    cmd.replace("。", "");
    cmd.replace("！", "");
    cmd.replace("？", "");

    m_logView->append(QString("[语音] 识别: %1").arg(cmd));

    // --- 回合切换命令 ---
    if (cmd == "我的回合" || cmd == "该我了" || cmd == "我") {
        m_manualTurnPlayer = 0;
        m_statusLabel->setText("当前回合: 你 | 语音面板已启用");
        m_logView->append("  -> 切换到你的回合");
        return;
    }
    if (cmd == "下家回合" || cmd == "下家") {
        m_manualTurnPlayer = 1;
        m_statusLabel->setText("当前回合: 下家 | 语音面板已启用");
        m_logView->append("  -> 切换到下家回合");
        return;
    }
    if (cmd == "对家回合" || cmd == "对家") {
        m_manualTurnPlayer = 2;
        m_statusLabel->setText("当前回合: 对家 | 语音面板已启用");
        m_logView->append("  -> 切换到对家回合");
        return;
    }
    if (cmd == "上家回合" || cmd == "上家") {
        m_manualTurnPlayer = 3;
        m_statusLabel->setText("当前回合: 上家 | 语音面板已启用");
        m_logView->append("  -> 切换到上家回合");
        return;
    }

    // 自操作辅助函数: 检查是否是玩家的回合
    auto guardSelfTurn = [&]() -> bool {
        if (m_manualTurnPlayer != 0) {
            const char *tn[] = {"你", "下家", "对家", "上家"};
            m_logView->append(QString("  [阻止] 现在是%1的回合，不是你的回合").arg(tn[m_manualTurnPlayer]));
            return false;
        }
        return true;
    };

    // --- 命令解析 ---
    // 摸牌: 摸/摸到 + 牌
    if (cmd.startsWith("摸到")) {
        cmd = cmd.mid(2);
    }
    if (cmd.startsWith("摸")) {
        if (!guardSelfTurn()) return;
        QString tileStr = cmd.mid(1);
        std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
        if (tiles.size() == 1) {
            PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
            ps.hand.push_back(tiles[0]);
            std::sort(ps.hand.begin(), ps.hand.end());
            m_engine->checkDingqueDone(0);
            m_logView->append(QString("  -> 摸到了 %1").arg(tiles[0].toString()));
            refreshAllUI();
            refreshSuggestions();
            refreshOpponentProfiles();
        } else {
            m_logView->append("  [错误] 无法识别牌面，格式: 摸3筒 或 摸到3筒");
        }
        return;
    }

    // 打出/出牌: 打/打出 + 牌
    if (cmd.startsWith("打出")) {
        cmd = cmd.mid(2);
    }
    if (cmd.startsWith("打")) {
        if (!guardSelfTurn()) return;
        QString tileStr = cmd.mid(1);
        std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
        if (tiles.size() == 1) {
            PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
            bool ok = MahjongLogic::removeTile(ps.hand, tiles[0]);
            if (ok) {
                ps.discards.push_back(tiles[0]);
                m_engine->checkDingqueDone(0);
                m_logView->append(QString("  -> 打出了 %1").arg(tiles[0].toString()));
                refreshAllUI();
                refreshSuggestions();
                refreshOpponentProfiles();
            } else {
                m_logView->append("  [错误] 手牌中没有这张牌");
            }
        } else {
            m_logView->append("  [错误] 无法识别牌面，格式: 打3筒 或 打出3筒");
        }
        return;
    }

    // 对手出牌: 下家/对家/上家 + 打/出 + 牌
    int oppPlayer = -1;
    if (cmd.startsWith("下家")) {
        oppPlayer = 1; cmd = cmd.mid(2);
    } else if (cmd.startsWith("对家")) {
        oppPlayer = 2; cmd = cmd.mid(2);
    } else if (cmd.startsWith("上家")) {
        oppPlayer = 3; cmd = cmd.mid(2);
    }
    if (oppPlayer >= 0) {
        if (cmd.startsWith("打")) cmd = cmd.mid(1);
        if (cmd.startsWith("出")) cmd = cmd.mid(1);
        std::vector<Tile> tiles = MahjongLogic::parseTileList(cmd);
        if (tiles.size() == 1) {
            PlayerState &ops = const_cast<PlayerState &>(m_engine->playerState(oppPlayer));
            ops.discards.push_back(tiles[0]);
            m_logView->append(QString("  -> 玩家%1 打出了 %2")
                .arg(oppPlayer + 1).arg(tiles[0].toString()));
            refreshOpponentUI(mapPlayerToSeat(oppPlayer));
            refreshOpponentProfiles();
        } else {
            m_logView->append("  [错误] 格式: 下家打3筒 或 对家打5万");
        }
        return;
    }

    // 碰: 碰/碰牌 + 牌
    if (cmd.startsWith("碰") && cmd.size() > 1) {
        if (!guardSelfTurn()) return;
        QString tileStr = cmd.mid(1);
        std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
        if (tiles.size() == 1) {
            PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
            std::vector<Tile> pongTiles;
            pongTiles.push_back(tiles[0]);
            int removed = 0;
            auto it = ps.hand.begin();
            while (it != ps.hand.end() && removed < 2) {
                if (*it == tiles[0]) {
                    pongTiles.push_back(*it);
                    it = ps.hand.erase(it);
                    removed++;
                } else { ++it; }
            }
            if (removed >= 2) {
                ps.melds.push_back(pongTiles);
                m_logView->append(QString("  -> 碰了 %1").arg(tiles[0].toString()));
                refreshAllUI();
            } else {
                m_logView->append("  [错误] 手牌中不足2张");
            }
        } else {
            m_logView->append("  [错误] 格式: 碰3筒");
        }
        return;
    }

    // 杠: 暗杠/加杠/明杠/杠 + 牌
    int kongType = 0; // 0=none, 1=exposed, 2=hidden, 3=add
    QString kongCmd = cmd;
    if (kongCmd.startsWith("暗杠")) {
        kongType = 2; kongCmd = kongCmd.mid(2);
    } else if (kongCmd.startsWith("加杠")) {
        kongType = 3; kongCmd = kongCmd.mid(2);
    } else if (kongCmd.startsWith("明杠")) {
        kongType = 1; kongCmd = kongCmd.mid(2);
    } else if (kongCmd.startsWith("杠") && kongCmd.size() > 1) {
        kongType = 1; kongCmd = kongCmd.mid(1);
    }
    if (kongType > 0) {
        if (!guardSelfTurn()) return;
        std::vector<Tile> tiles = MahjongLogic::parseTileList(kongCmd);
        if (tiles.size() == 1) {
            Tile t = tiles[0];
            PlayerState &ps = const_cast<PlayerState &>(m_engine->playerState(0));
            if (kongType == 2) {
                // 暗杠：手牌中4张
                std::vector<Tile> kongTiles;
                int removed = 0;
                auto it = ps.hand.begin();
                while (it != ps.hand.end() && removed < 4) {
                    if (*it == t) { kongTiles.push_back(*it); it = ps.hand.erase(it); removed++; }
                    else { ++it; }
                }
                if (removed >= 4) {
                    ps.melds.push_back(kongTiles);
                    m_logView->append(QString("  -> 暗杠了 %1").arg(t.toString()));
                    refreshAllUI();
                } else {
                    m_logView->append("  [错误] 手牌中该牌不足4张");
                }
            } else if (kongType == 3) {
                // 加杠：已碰的牌中加一张
                bool found = false;
                for (auto &meld : ps.melds) {
                    if (meld.size() == 3 && meld[0] == t) {
                        meld.push_back(t);
                        MahjongLogic::removeTile(ps.hand, t);
                        found = true;
                        break;
                    }
                }
                if (found) {
                    m_logView->append(QString("  -> 加杠了 %1").arg(t.toString()));
                    refreshAllUI();
                } else {
                    m_logView->append(QString("  [错误] 没有找到已碰的 %1").arg(t.toString()));
                }
            } else {
                // 明杠：手牌中3张
                std::vector<Tile> kongTiles;
                kongTiles.push_back(t);
                int removed = 0;
                auto it = ps.hand.begin();
                while (it != ps.hand.end() && removed < 3) {
                    if (*it == t) { kongTiles.push_back(*it); it = ps.hand.erase(it); removed++; }
                    else { ++it; }
                }
                if (removed >= 3) {
                    ps.melds.push_back(kongTiles);
                    m_logView->append(QString("  -> 杠了 %1 (明杠)").arg(t.toString()));
                    refreshAllUI();
                } else {
                    m_logView->append("  [错误] 手牌中该牌不足3张");
                }
            }
        } else {
            m_logView->append("  [错误] 格式: 杠3筒 / 明杠3筒 / 暗杠3筒 / 加杠3筒");
        }
        return;
    }

    // 胡牌: 胡/自摸/胡了
    if (cmd == "胡" || cmd == "自摸" || cmd == "胡了") {
        if (!guardSelfTurn()) return;
        if (m_engine->canSelfWin(0)) {
            m_engine->playerSelfWin(0);
            m_logView->append("  -> 自摸胡了!");
        } else {
            m_logView->append("  [错误] 当前手牌未满足胡牌条件");
        }
        refreshAllUI();
        return;
    }

    // 点炮胡
    if (cmd.startsWith("胡") && cmd.size() > 1) {
        if (!guardSelfTurn()) return;
        QString who = cmd.mid(1);
        int from = -1;
        if (who == "下家") from = 1;
        else if (who == "对家") from = 2;
        else if (who == "上家") from = 3;
        if (from >= 0 && m_engine->canWinOnDiscard(0)) {
            m_engine->playerWinOnDiscard(0);
            m_logView->append(QString("  -> 胡了玩家%1 的炮!").arg(from + 1));
            refreshAllUI();
        } else {
            m_logView->append("  [错误] 未满足点炮胡条件或无法识别");
        }
        return;
    }

    // 对手碰/杠
    for (int i = 1; i <= 3; ++i) {
        QString prefix;
        if (i == 1) prefix = "下家";
        else if (i == 2) prefix = "对家";
        else prefix = "上家";

        if (cmd.startsWith(prefix + "碰")) {
            QString tileStr = cmd.mid(prefix.size() + 1);
            std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
            if (tiles.size() == 1) {
                PlayerState &ops = const_cast<PlayerState &>(m_engine->playerState(i));
                std::vector<Tile> meldTiles(3, tiles[0]);
                ops.melds.push_back(meldTiles);
                m_logView->append(QString("  -> 玩家%1 碰了 %2").arg(i + 1).arg(tiles[0].toString()));
                refreshOpponentUI(mapPlayerToSeat(i));
                refreshOpponentProfiles();
            }
            return;
        }
        if (cmd.startsWith(prefix + "杠")) {
            QString tileStr = cmd.mid(prefix.size() + 1);
            std::vector<Tile> tiles = MahjongLogic::parseTileList(tileStr);
            if (tiles.size() == 1) {
                PlayerState &ops = const_cast<PlayerState &>(m_engine->playerState(i));
                std::vector<Tile> meldTiles(4, tiles[0]);
                ops.melds.push_back(meldTiles);
                m_logView->append(QString("  -> 玩家%1 杠了 %2").arg(i + 1).arg(tiles[0].toString()));
                refreshOpponentUI(mapPlayerToSeat(i));
                refreshOpponentProfiles();
            }
            return;
        }
    }

    // 无法识别
    m_logView->append("  [提示] 支持指令: 摸3筒 / 打5万 / 碰8条 / 杠2筒 / 暗杠2筒 / 加杠2筒 / 胡 / 下家打4条 / 对家碰5万");
}

void CombatArenaWidget::onPeekOpponent(int player) {
    if (m_mode == ArenaMode::RealtimeAssist) {
        m_logView->append("[查看手牌] 实战中无法获知对方手牌!");
        return;
    }

    if (m_engine->phase() != GameEngine::Finished) {
        m_logView->append("[查看手牌] 游戏结束前禁止查看其他家手牌!");
        return;
    }

    m_logView->append(QString("[查看手牌] 正在查看玩家%1 的手牌...").arg(player + 1));

    if (m_engine->phase() == GameEngine::Idle) {
        m_logView->append("请先开始新游戏!");
        return;
    }
    const PlayerState &ps = m_engine->playerState(player);
    if (ps.isHuman) {
        m_logView->append("无法查看真人玩家的手牌!");
        return;
    }
    if (ps.hasWon) {
        m_logView->append(QString("玩家%1 已胡牌，无法查看!").arg(player + 1));
        return;
    }
    if (ps.hand.empty()) {
        m_logView->append(QString("玩家%1 没有手牌!").arg(player + 1));
        return;
    }

    // 使用 nullptr 父窗口确保弹窗置顶
    QDialog *dlg = new QDialog(nullptr);
    dlg->setWindowTitle(QString("对手%1 的手牌").arg(player + 1));
    dlg->setMinimumWidth(420);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowFlags(dlg->windowFlags() | Qt::WindowStaysOnTopHint);

    QVBoxLayout *dlgLayout = new QVBoxLayout(dlg);
    dlgLayout->setSpacing(12);

    QLabel *headerLabel = new QLabel(
        QString("<b>玩家%1 的手牌</b> (%2张)")
            .arg(player + 1)
            .arg(ps.hand.size()),
        dlg);
    headerLabel->setStyleSheet("font-size: 14px;");
    dlgLayout->addWidget(headerLabel);

    QScrollArea *scroll = new QScrollArea(dlg);
    scroll->setWidgetResizable(true);
    scroll->setMaximumHeight(90);
    QWidget *handContainer = new QWidget(dlg);
    QHBoxLayout *handLayout = new QHBoxLayout(handContainer);
    handLayout->setSpacing(3);
    handLayout->setContentsMargins(4, 4, 4, 4);

    for (const auto &t : ps.hand) {
        QPushButton *btn = new QPushButton(dlg);
        QPixmap pix = tileIcon(t);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(QSize(46, 56));
        } else {
            btn->setText(tileToDisplay(t));
        }
        btn->setFixedSize(46, 56);
        btn->setEnabled(false);
        handLayout->addWidget(btn);
    }
    handLayout->addStretch();
    scroll->setWidget(handContainer);
    dlgLayout->addWidget(scroll);

    QLabel *info = new QLabel(
        QString("定缺: %1 | 手牌数: %2 | 已胡: %3")
            .arg(ps.dingqueSet
                ? (ps.dingque == TileSuit::Dot ? "缺筒" : (ps.dingque == TileSuit::Bamboo ? "缺条" : "缺万"))
                : "未定")
            .arg(ps.hand.size())
            .arg(ps.hasWon ? "是" : "否"),
        dlg);
    info->setStyleSheet("font-size: 12px; color: #aaa;");
    dlgLayout->addWidget(info);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok, dlg);
    connect(btns, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    dlgLayout->addWidget(btns);

    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

// ===== 引擎信号处理 =====

void CombatArenaWidget::onGameStarted() {
    m_logView->clear();
    hideDingquePanel();
    m_actionPanel->hide();

    if (m_mode == ArenaMode::AISimulation) {
        // AI 自动定缺
        for (int i = 1; i < 4; ++i) {
            TileSuit dq = m_ai->chooseDingque(m_engine->playerState(i).hand);
            m_engine->setDingque(i, dq);
        }
        // 人类玩家需定缺
        showDingquePanel();
        m_statusLabel->setText("请选择定缺花色");
    }

    refreshAllUI();
}

void CombatArenaWidget::onTurnChanged(int player) {
    refreshAllUI();

    if (player == 0) {
        const PlayerState &ps = m_engine->playerState(player);
        QString dqStr;
        if (ps.dingqueSet) {
            dqStr = ps.dingque == TileSuit::Dot ? "缺筒" : (ps.dingque == TileSuit::Bamboo ? "缺条" : "缺万");
            m_statusLabel->setText(QString("你的回合: 点击手牌打出 | %1").arg(dqStr));
        } else {
            m_statusLabel->setText("你的回合: 点击手牌打出");
        }
        // 摸牌后刷新出牌建议
        refreshSuggestions();
        refreshOpponentProfiles();
    } else {
        m_statusLabel->setText(QString("对手%1 的回合...").arg(player + 1));
    }

    // 实战模式不自动执行 AI 回合，等待用户手动输入牌局进展
    if (m_mode == ArenaMode::RealtimeAssist) return;

    if (player != 0 && !m_engine->playerState(player).isHuman) {
        const auto &hand = m_engine->playerState(player).hand;
        if (m_engine->canSelfWin(player)) {
            m_engine->playerSelfWin(player);
        } else if (m_engine->canSelfKong(player)) {
            const auto &melds = m_engine->playerState(player).melds;
            if (m_ai->shouldSelfKong(hand, melds)) {
                std::vector<int> counts = MahjongLogic::tilesToCounts(hand);
                for (int i = 0; i < 30; ++i) {
                    if (counts[i] >= 4) {
                        Tile tile = {static_cast<TileSuit>(i / 10), i % 10};
                        m_engine->playerSelfKong(player, tile, GameEngine::HiddenKong);
                        refreshAllUI();
                        m_aiTimer->start();
                        return;
                    }
                }
                for (const auto &meld : melds) {
                    if (meld.size() == 3) {
                        for (const auto &t : hand) {
                            if (t == meld[0]) {
                                m_engine->playerSelfKong(player, t, GameEngine::AddKong);
                                refreshAllUI();
                                m_aiTimer->start();
                                return;
                            }
                        }
                    }
                }
            }
        }
        m_aiTimer->start();
    }
}

void CombatArenaWidget::onPlayerDiscarded(int player, Tile tile) {
    Q_UNUSED(tile);
    refreshAllUI();
    refreshSuggestions();
    refreshOpponentProfiles();

    if (m_mode == ArenaMode::RealtimeAssist) return;

    if (player != 0) {
        refreshActionButtons();
        if (m_engine->phase() == GameEngine::Playing) {
            m_aiTimer->start();
        }
    } else {
        m_aiTimer->start();
    }
}

void CombatArenaWidget::onPlayerPonged(int player, Tile tile) {
    Q_UNUSED(tile);
    refreshAllUI();
    refreshOpponentProfiles();
    if (m_mode == ArenaMode::RealtimeAssist) return;
    if (player != 0 && !m_engine->playerState(player).isHuman) {
        m_aiTimer->start();
    }
}

void CombatArenaWidget::onPlayerKonged(int player, Tile tile, GameEngine::KongType kongType) {
    Q_UNUSED(tile);
    Q_UNUSED(kongType);
    refreshAllUI();
    refreshOpponentProfiles();
    if (m_mode == ArenaMode::RealtimeAssist) return;
    if (player != 0 && !m_engine->playerState(player).isHuman) {
        m_aiTimer->start();
    }
}

void CombatArenaWidget::onPlayerWon(int player, bool selfDrawn, int fan) {
    refreshAllUI();
    refreshSuggestions();
    m_statusLabel->setText(QString("玩家%1 %2! (%3番)")
        .arg(player + 1)
        .arg(selfDrawn ? "自摸" : "胡")
        .arg(fan));

    if (m_mode == ArenaMode::RealtimeAssist) return;
    if (m_engine->phase() == GameEngine::Playing) {
        m_aiTimer->start();
    }
}

void CombatArenaWidget::onDingqueSet(int player, TileSuit suit) {
    QString name = suit == TileSuit::Dot ? "缺筒"
        : (suit == TileSuit::Bamboo ? "缺条" : "缺万");
    if (player == 0) return; // 自己的定缺由 onTurnChanged 显示
    m_oppDingqueLabels[player]->setText(name);
}

void CombatArenaWidget::onGameOver() {
    m_statusLabel->setText("对局结束! 点击「新游戏」再来一局");
    m_actionPanel->hide();
    hideDingquePanel();
    refreshAllUI();
}

void CombatArenaWidget::onLogMessage(const QString &msg) {
    m_logView->append(msg);
}

// ===== AI 定时器 =====

void CombatArenaWidget::onAITimerTick() {
    runAITurn();
}

// ===== UI 刷新 =====

void CombatArenaWidget::refreshAllUI() {
    // 刷新对手区域
    for (int i = 1; i < 4; ++i) {
        refreshOpponentUI(i);
    }
    // 刷新自家面子
    refreshSelfMeld();
    // 刷新手牌
    refreshHandUI();
    // 牌墙
    m_wallLabel->setText(QString("牌墙: %1").arg(m_engine->tilesRemaining()));
    // 最后打出的牌
    if (m_engine->lastDiscardPlayer() >= 0) {
        m_lastDiscardLabel->setText(
            QString("最后打出:<br>%1<br>(玩家%2)")
                .arg(tileToHtml(m_engine->lastDiscard(), 30, 42))
                .arg(m_engine->lastDiscardPlayer() + 1));
    }
}

void CombatArenaWidget::refreshOpponentUI(int player) {
    const PlayerState &ps = m_engine->playerState(player);

    // 面子
    QString meldStr;
    for (const auto &meld : ps.melds) {
        meldStr += "[";
        for (const auto &t : meld) {
            meldStr += tileToHtml(t, 18, 26);
        }
        meldStr += "] ";
    }
    m_oppMeldLabels[player]->setText(meldStr.isEmpty() ? "" : "面子: " + meldStr);

    // 弃牌
    QString discardStr = "弃牌: ";
    for (const auto &t : ps.discards) {
        discardStr += tileToHtml(t, 18, 26) + " ";
    }
    m_oppDiscardLabels[player]->setText(discardStr);

    // 查看手牌按钮 — 只允许在模拟模式且对局已结束后查看
    bool canPeek = (m_mode == ArenaMode::AISimulation && m_engine->phase() == GameEngine::Finished);
    m_oppPeekButtons[player]->setEnabled(canPeek);

    // 定缺
    if (ps.dingqueSet) {
        QString dq = ps.dingque == TileSuit::Dot ? "缺筒"
            : (ps.dingque == TileSuit::Bamboo ? "缺条" : "缺万");
        m_oppDingqueLabels[player]->setText(
            QString("定缺: %1 %2").arg(dq).arg(ps.dingqueDone ? "✓" : ""));
    }

    // 对手框高亮样式 (保留基础样式再加高亮边框)
    if (ps.hasWon) {
        m_opponentBoxes[player]->setStyleSheet(
            "QGroupBox { background-color: #f5f5f5; border: 2px solid red; "
            "border-radius: 6px; padding: 8px 10px 8px 10px; }");
    } else if (m_engine->phase() == GameEngine::Playing
               && m_engine->currentPlayer() == player) {
        m_opponentBoxes[player]->setStyleSheet(
            "QGroupBox { background-color: #f5f5f5; border: 2px solid #ddaa00; "
            "border-radius: 6px; padding: 8px 10px 8px 10px; }");
    } else {
        m_opponentBoxes[player]->setStyleSheet(
            "QGroupBox { background-color: #f5f5f5; border: 1px solid #ccc; "
            "border-radius: 6px; padding: 8px 10px 8px 10px; }");
    }
}

void CombatArenaWidget::refreshSelfMeld() {
    const PlayerState &ps = m_engine->playerState(0);
    if (ps.melds.empty()) {
        m_selfMeldLabel->setText("");
        m_selfMeldLabel->setVisible(false);
        return;
    }
    m_selfMeldLabel->setVisible(true);

    QString text;
    for (const auto &meld : ps.melds) {
        if (meld.size() == 4) {
            // 4张可能是明杠/加杠/暗杠 — 通过检查是否有3张相同面+1张补牌判断
            // 简化：4张相同→显示为杠
            text += "[";
            for (const auto &t : meld) {
                text += tileToHtml(t, 18, 26);
            }
            text += "] ";
        } else if (meld.size() == 3) {
            // 碰
            text += "[";
            for (const auto &t : meld) {
                text += tileToHtml(t, 18, 26);
            }
            text += "] ";
        }
    }
    m_selfMeldLabel->setText("副露: " + text);
}

void CombatArenaWidget::refreshHandUI() {
    // 清空旧手牌
    while (m_handLayout->count() > 0) {
        QLayoutItem *item = m_handLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_handButtons.clear();

    const PlayerState &ps = m_engine->playerState(0);
    m_displayHand = ps.hand;

    bool isTurn = isHumanTurn() && m_engine->phase() == GameEngine::Playing;
    // 定缺规则: 缺门未打完时，非缺门花色不能打
    bool mustPlayDingque = isTurn && ps.dingqueSet && !ps.dingqueDone;

    for (size_t i = 0; i < ps.hand.size(); ++i) {
        Tile tile = ps.hand[i];
        QPushButton *btn = new QPushButton(this);
        QPixmap pix = tileIcon(tile);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(QSize(50, 70));
        } else {
            btn->setText(tileToDisplay(tile));
        }
        btn->setFixedSize(56, 82);

        bool canDiscard = isTurn && (!mustPlayDingque || tile.suit == ps.dingque);
        btn->setEnabled(canDiscard);

        QString color = suitColor(tile.suit);
        if (canDiscard) {
            btn->setStyleSheet(QString(
                "QPushButton { background-color: %1; color: white; font-size: 14px; "
                "border: 1px solid #555; border-radius: 6px; }"
                "QPushButton:hover:enabled { border: 2px solid #fff; }"
            ).arg(color));
        } else if (mustPlayDingque && tile.suit != ps.dingque) {
            btn->setStyleSheet(
                "QPushButton { background-color: #3a3a3a; color: #666; font-size: 14px; "
                "border: 1px solid #444; border-radius: 6px; }"
                "QPushButton:disabled { background-color: #3a3a3a; color: #666; }");
            btn->setToolTip(QString("定缺规则: 必须优先打出%1花色!")
                .arg(ps.dingque == TileSuit::Dot ? "筒" : (ps.dingque == TileSuit::Bamboo ? "条" : "万")));
        } else {
            btn->setStyleSheet(QString(
                "QPushButton { background-color: %1; color: white; font-size: 14px; "
                "border: 1px solid #555; border-radius: 6px; }"
                "QPushButton:disabled { background-color: #666; color: #aaa; }"
            ).arg(color));
        }

        int idx = static_cast<int>(i);
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            onHandTileClicked(idx);
        });
        m_handLayout->addWidget(btn);
        m_handButtons.append(btn);
    }
    m_handLayout->addStretch();
}

void CombatArenaWidget::refreshActionButtons() {
    if (m_engine->phase() != GameEngine::Playing) {
        m_actionPanel->hide();
        return;
    }

    if (!isHumanTurn()) {
        m_actionPanel->hide();
        return;
    }

    int lp = m_engine->lastDiscardPlayer();
    if (lp < 0 || lp == 0) {
        // 人类可执行暗杠/自摸
        bool canSK = false;
        bool canSW = false;
        if (m_mode == ArenaMode::RealtimeAssist) {
            // 实时模式直接检查，绕过引擎的 m_currentPlayer 条件
            const PlayerState &ps = m_engine->playerState(0);
            // 暗杠/加杠
            std::vector<int> counts = MahjongLogic::tilesToCounts(ps.hand);
            for (int i = 0; i < 30; ++i) {
                if (counts[i] >= 4) { canSK = true; break; }
            }
            if (!canSK) {
                for (const auto &meld : ps.melds) {
                    if (meld.size() == 3) {
                        for (const auto &t : ps.hand) {
                            if (t == meld[0]) { canSK = true; break; }
                        }
                        if (canSK) break;
                    }
                }
            }
            // 自摸
            canSW = !ps.dingqueSet || ps.dingqueDone;
            if (canSW) canSW = MahjongLogic::canWin(ps.hand);
        } else {
            canSK = m_engine->canSelfKong(0);
            canSW = m_engine->canSelfWin(0);
        }
        if (canSK || canSW) {
            m_selfKongBtn->setVisible(canSK);
            m_winBtn->setVisible(canSW);
            m_pongBtn->hide();
            m_kongBtn->hide();
            m_actionPanel->show();
        } else {
            m_actionPanel->hide();
        }
        return;
    }

    // 其他玩家出牌，检查人类是否可以响应
    bool canP = m_engine->canPong(0);
    bool canK = m_engine->canExposedKong(0);
    bool canW = m_engine->canWinOnDiscard(0);

    if (canP || canK || canW) {
        m_pongBtn->setVisible(canP);
        m_kongBtn->setVisible(canK);
        m_winBtn->setVisible(canW);
        m_selfKongBtn->hide();
        m_actionPanel->show();
    } else {
        m_actionPanel->hide();
    }
}

void CombatArenaWidget::refreshSuggestions() {
    m_suggestionBox->clear();
    const PlayerState &ps = m_engine->playerState(0);
    if (ps.hand.empty() || ps.hasWon) {
        if (ps.hasWon) m_suggestionBox->append("【已胡牌】");
        return;
    }

    // 定缺未完成: 必须先打缺门牌，向听数需加上剩余缺门牌数
    int dingqueRemaining = 0;
    if (ps.dingqueSet && !ps.dingqueDone) {
        for (const auto &t : ps.hand) {
            if (t.suit == ps.dingque) dingqueRemaining++;
        }
    }

    if (dingqueRemaining > 0) {
        QString dqName = ps.dingque == TileSuit::Dot ? "筒" : (ps.dingque == TileSuit::Bamboo ? "条" : "万");
        m_suggestionBox->append(QString("<b>定缺未完成: 还有 %1 张%2必须优先打出</b>")
            .arg(dingqueRemaining).arg(dqName));

        // 用完整手牌算向听数(数学上可能已听牌)，再加上缺门牌数惩罚
        int rawShanten = MahjongLogic::calculateShanten(ps.hand);
        int effectiveShanten = std::max(0, rawShanten) + dingqueRemaining;
        m_suggestionBox->append(QString("当前有效向听数: <b>%1</b> (缺门打完前无法胡牌)")
            .arg(effectiveShanten));

        // 推荐优先打出缺门中的孤立牌/边张
        m_suggestionBox->append("<b>--- 优先打出缺门牌 ---</b>");

        // 收集缺门中的不同牌，按优先级排序
        std::vector<Tile> dqTiles;
        for (const auto &t : ps.hand) {
            if (t.suit == ps.dingque) dqTiles.push_back(t);
        }
        std::sort(dqTiles.begin(), dqTiles.end());

        struct DqOption { Tile tile; bool isolated; bool edge; };
        std::vector<DqOption> dqOptions;
        for (const auto &tile : dqTiles) {
            bool isolated = true;
            for (const auto &t : dqTiles) {
                if (t.suit == tile.suit && t.value != tile.value
                    && std::abs(t.value - tile.value) <= 2) {
                    isolated = false;
                    break;
                }
            }
            bool edge = (tile.value == 1 || tile.value == 9);
            dqOptions.push_back({tile, isolated, edge});
        }

        std::sort(dqOptions.begin(), dqOptions.end(),
            [](const DqOption &a, const DqOption &b) {
                if (a.isolated != b.isolated) return a.isolated > b.isolated;
                if (a.edge != b.edge) return a.edge > b.edge;
                return a.tile < b.tile;
            });

        for (const auto &opt : dqOptions) {
            QString tags;
            if (opt.isolated) tags += "[孤立]";
            if (opt.edge) tags += "[边张]";
            QString line = QString("  打 <b>%1</b> %2")
                .arg(opt.tile.toString()).arg(tags);
            m_suggestionBox->append(line);
        }
        return;
    }

    // 13张手牌时，直接显示可胡牌目标
    if (ps.hand.size() == 13) {
        int shanten = MahjongLogic::calculateShanten(ps.hand);
        m_suggestionBox->append(QString("<b>当前向听数: %1</b>")
            .arg(shanten == -1 ? "已听牌!" : QString::number(shanten)));

        auto waits = MahjongLogic::getWaits(ps.hand);
        if (!waits.empty()) {
            QString waitStr;
            for (const auto &w : waits) {
                waitStr += w.toString() + " ";
            }
            int waitTiles = MahjongLogic::countRemainingWaitTiles(ps.hand, waits);
            m_suggestionBox->append(QString("<b>可胡牌目标 (%1 张):</b> %2")
                .arg(waitTiles).arg(waitStr));
        } else {
            m_suggestionBox->append("当前未听牌，暂时没有直接胡牌的目标牌。\n请继续摸牌或调整手牌结构。");
        }
        return;
    }

    // 定缺已完成，正常分析
    int shanten = MahjongLogic::calculateShanten(ps.hand);
    m_suggestionBox->append(QString("<b>当前向听数: %1</b>")
        .arg(shanten == -1 ? "已听牌!" : QString::number(shanten)));

    // 计算打牌建议所需的听牌列表
    auto suggestions = MahjongLogic::getDiscardSuggestions(ps.hand);

    if (shanten == -1) {
        m_suggestionBox->append("等待自摸即可胡牌");
        auto waits = MahjongLogic::getWaits(ps.hand);
        if (!waits.empty()) {
            QString waitStr;
            for (const auto &w : waits) {
                waitStr += w.toString() + " ";
            }
            int waitTiles = MahjongLogic::countRemainingWaitTiles(ps.hand, waits);
            m_suggestionBox->append(QString("听 %1 张: %2").arg(waitTiles).arg(waitStr));
        }
        int maxWaits = 0;
        for (const auto &pair : suggestions) {
            std::vector<Tile> afterDiscard = ps.hand;
            MahjongLogic::removeTile(afterDiscard, pair.first);
            int count = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
            if (count > maxWaits) maxWaits = count;
        }

        m_suggestionBox->append("<b>--- 推荐出牌 (可听牌) ---</b>");
        for (const auto &pair : suggestions) {
            std::vector<Tile> afterDiscard = ps.hand;
            MahjongLogic::removeTile(afterDiscard, pair.first);
            int waitCount = MahjongLogic::countRemainingWaitTiles(afterDiscard, pair.second);
            int newShanten = MahjongLogic::calculateShanten(afterDiscard);

            int expectedFan = 1;
            for (const auto &wait : pair.second) {
                std::vector<Tile> winHand = afterDiscard;
                winHand.push_back(wait);
                std::sort(winHand.begin(), winHand.end());
                int fan = MahjongLogic::calculateFanTypes(winHand);
                if (fan > expectedFan) expectedFan = fan;
            }

            bool isBest = (waitCount == maxWaits);
            QString line = QString("%1 打 <b>%2</b> → 听 %3 张 [向听:%4 预期:%5番]")
                .arg(isBest ? "★" : " ")
                .arg(pair.first.toString())
                .arg(waitCount)
                .arg(newShanten)
                .arg(expectedFan);

            if (isBest) {
                m_suggestionBox->append(QString("<span style='color:#D77757;'>%1</span>").arg(line));
            } else {
                m_suggestionBox->append(line);
            }

            QString waitList;
            for (const auto &w : pair.second) {
                waitList += w.toString() + " ";
            }
            m_suggestionBox->append(QString("  听牌: %1").arg(waitList));
        }
    } else {
        m_suggestionBox->append("<b>--- 推荐出牌 (优化向听) ---</b>");

        std::vector<Tile> uniqueTiles = ps.hand;
        std::sort(uniqueTiles.begin(), uniqueTiles.end());
        uniqueTiles.erase(std::unique(uniqueTiles.begin(), uniqueTiles.end()), uniqueTiles.end());

        struct DiscardOption {
            Tile tile; int shanten; bool isolated; bool isDingque; bool isEdge;
        };
        std::vector<DiscardOption> options;

        for (const auto &tile : uniqueTiles) {
            std::vector<Tile> hand13 = ps.hand;
            MahjongLogic::removeTile(hand13, tile);
            int s = MahjongLogic::calculateShanten(hand13);
            bool isolated = MahjongLogic::isIsolated(hand13, tile);
            bool isDq = ps.dingqueSet && tile.suit == ps.dingque;
            bool edge = (tile.value == 1 || tile.value == 9);
            options.push_back({tile, s, isolated, isDq, edge});
        }

        std::sort(options.begin(), options.end(),
            [](const DiscardOption &a, const DiscardOption &b) {
                if (a.shanten != b.shanten) return a.shanten < b.shanten;
                if (a.isolated != b.isolated) return a.isolated > b.isolated;
                if (a.isDingque != b.isDingque) return a.isDingque > b.isDingque;
                if (a.isEdge != b.isEdge) return a.isEdge > b.isEdge;
                return a.tile < b.tile;
            });

        int showCount = std::min(static_cast<int>(options.size()), 6);
        int bestShanten = options.empty() ? 99 : options[0].shanten;

        for (int i = 0; i < showCount; ++i) {
            const auto &opt = options[i];
            bool isBest = (opt.shanten == bestShanten);
            QString tags;
            if (opt.isolated) tags += "[孤立]";
            if (opt.isDingque) tags += "[缺门]";
            if (opt.isEdge) tags += "[边张]";
            QString line = QString("%1 打 <b>%2</b> → 向听:%3 %4")
                .arg(isBest ? "★" : " ")
                .arg(opt.tile.toString())
                .arg(opt.shanten)
                .arg(tags);
            if (isBest) {
                m_suggestionBox->append(QString("<span style='color:#D77757;'>%1</span>").arg(line));
            } else {
                m_suggestionBox->append(line);
            }
        }
    }
}

void CombatArenaWidget::refreshOpponentProfiles() {
    // profileLabel index: 0=上家, 1=下家, 2=对家
    // player index: 1=下家, 2=对家, 3=上家
    int profileToPlayer[3] = {3, 1, 2};

    for (int pi = 0; pi < 3; ++pi) {
        int player = profileToPlayer[pi];
        const PlayerState &ps = m_engine->playerState(player);

        if (m_engine->phase() == GameEngine::Idle) {
            m_profileStyleLabel[pi]->setText("风格: 等待游戏开始...");
            m_profileDangerLabel[pi]->setText("高危炮牌: --");
            continue;
        }

        int discardCount = static_cast<int>(ps.discards.size());

        // ===== 风格分析 =====
        // 统计各花色弃牌和中张弃牌
        int suitDiscards[3] = {0, 0, 0};
        int midDiscards = 0;   // 中张(3-7)
        for (const auto &t : ps.discards) {
            suitDiscards[static_cast<int>(t.suit)]++;
            if (t.value >= 3 && t.value <= 7) midDiscards++;
        }

        QString style;
        if (discardCount == 0) {
            style = "尚未出牌";
        } else if (discardCount == 1) {
            // 第一张就打出中张 → 偏进攻；边张 → 偏防守
            style = midDiscards > 0 ? "初步判断: 偏进攻" : "初步判断: 偏防守";
        } else {
            double midRatio = static_cast<double>(midDiscards) / discardCount;
            if (midRatio > 0.5)
                style = "进攻型（大量中张弃牌）";
            else if (midRatio < 0.25)
                style = "防守型（偏爱边张弃牌）";
            else
                style = "平衡型";
        }
        m_profileStyleLabel[pi]->setText(QString("风格: %1").arg(style));

        // ===== 危险牌分析 =====
        QStringList dangerList;

        // 非缺门花色 = 对手的留牌方向 → 对这些花色出牌更危险
        if (ps.dingqueSet && !ps.dingqueDone) {
            for (int s = 0; s < 3; ++s) {
                if (s == static_cast<int>(ps.dingque)) continue;
                QString suitName = s == 0 ? "筒" : (s == 1 ? "条" : "万");
                dangerList.append(QString("%1(对手留牌方向)").arg(suitName));
            }
        }

        // 缺门打完后的花色分析
        if (ps.dingqueDone && discardCount >= 2) {
            // 找出弃牌最少的花色 → 可能在做牌
            int minDiscardSuit = -1;
            int minCount = 999;
            for (int s = 0; s < 3; ++s) {
                if (suitDiscards[s] < minCount) {
                    minCount = suitDiscards[s];
                    minDiscardSuit = s;
                }
            }
            // 如果某花色弃牌占比 < 20%，怀疑在做清一色或混一色
            if (minDiscardSuit >= 0 && discardCount >= 3
                && suitDiscards[minDiscardSuit] < discardCount * 0.20) {
                QString suitName = minDiscardSuit == 0 ? "筒" : (minDiscardSuit == 1 ? "条" : "万");
                dangerList.append(QString("疑似%1清一色!").arg(suitName));
            }

            // 找出弃牌最少的花色中的高危张(4-7)
            if (minDiscardSuit >= 0 && suitDiscards[minDiscardSuit] <= 1) {
                QString suitName = minDiscardSuit == 0 ? "筒" : (minDiscardSuit == 1 ? "条" : "万");
                if (!dangerList.filter(suitName).isEmpty() || discardCount >= 3) {
                    dangerList.append(QString("%1中张(4-7%1)高风险").arg(suitName));
                }
            }
        }

        // 面子分析: 已碰/杠的花色确认危险
        if (!ps.melds.empty()) {
            for (const auto &meld : ps.melds) {
                if (meld.empty()) continue;
                // 面子中每张牌的 ±2 范围是高危区
                for (const auto &t : meld) {
                    int lo = std::max(1, t.value - 2);
                    int hi = std::min(9, t.value + 2);
                    QString suitName = t.suit == TileSuit::Dot ? "筒" : (t.suit == TileSuit::Bamboo ? "条" : "万");
                    QString range = QString("%1%2-%1%3").arg(lo).arg(suitName).arg(hi);
                    if (!dangerList.contains(range)) {
                        dangerList.append(range);
                    }
                }
            }
        }

        // 汇总
        if (dangerList.isEmpty()) {
            if (discardCount > 0)
                m_profileDangerLabel[pi]->setText("高危炮牌: 暂未检测到明显危险");
            else
                m_profileDangerLabel[pi]->setText("高危炮牌: 等待对手出牌...");
        } else {
            // 去重并限制条数
            dangerList.removeDuplicates();
            QString result;
            int showCount = std::min(static_cast<int>(dangerList.size()), 3);
            for (int i = 0; i < showCount; ++i) {
                if (i > 0) result += " | ";
                result += dangerList[i];
            }
            m_profileDangerLabel[pi]->setText(
                QString("高危炮牌: %1").arg(result));
        }
    }
}

void CombatArenaWidget::showDingquePanel() {
    m_dingquePanel->show();
}

void CombatArenaWidget::hideDingquePanel() {
    m_dingquePanel->hide();
}

// ===== 工具函数 =====

QString CombatArenaWidget::tileToDisplay(Tile t) const {
    return t.toString();
}

QString CombatArenaWidget::suitColor(TileSuit suit) const {
    switch (suit) {
        case TileSuit::Dot: return "#cc4444";
        case TileSuit::Bamboo: return "#44aa44";
        case TileSuit::Character: return "#4488cc";
    }
    return "#888888";
}

bool CombatArenaWidget::isHumanTurn() const {
    // 实时辅助模式下用 m_manualTurnPlayer 判断回合，而非引擎内部 currentPlayer
    // 因为引擎在每次弃牌后会自动推进 m_currentPlayer 到下家
    bool myTurn = (m_mode == ArenaMode::RealtimeAssist)
        ? (m_manualTurnPlayer == 0)
        : (m_engine->currentPlayer() == 0);
    return myTurn
        && !m_engine->playerState(0).hasWon
        && m_engine->phase() == GameEngine::Playing;
}

void CombatArenaWidget::runAITurn() {
    if (m_engine->phase() != GameEngine::Playing) return;

    int cp = m_engine->currentPlayer();
    int lp = m_engine->lastDiscardPlayer();

    // 出牌响应阶段：检查 AI 是否可以碰/杠/胡
    if (lp >= 0) {
        // 胡优先级最高
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;
            if (m_engine->canWinOnDiscard(i) && m_ai->shouldWin()) {
                m_engine->playerWinOnDiscard(i);
                refreshAllUI();
                return;
            }
        }
        // 杠
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;
            if (m_engine->canExposedKong(i)
                && m_ai->shouldKong(m_engine->lastDiscard(),
                                    m_engine->playerState(i).hand,
                                    m_engine->playerState(i).dingque)) {
                m_engine->playerKong(i);
                refreshAllUI();
                return;
            }
        }
        // 碰
        for (int i = 0; i < 4; ++i) {
            if (i == lp) continue;
            if (m_engine->playerState(i).hasWon) continue;
            if (m_engine->playerState(i).isHuman) continue;
            if (m_engine->canPong(i)
                && m_ai->shouldPong(m_engine->lastDiscard(),
                                    m_engine->playerState(i).hand,
                                    m_engine->playerState(i).dingque)) {
                m_engine->playerPong(i);
                refreshAllUI();
                return;
            }
        }
        // 人类可响应时等待输入
        if (m_engine->canWinOnDiscard(0)
            || m_engine->canExposedKong(0)
            || m_engine->canPong(0)) {
            return;
        }
        m_engine->advanceTurn();
        refreshAllUI();
        return;
    }

    // 正常回合
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
        m_engine->playerState(cp).dingque, ctx);

    m_engine->playerDiscard(discard);
    refreshAllUI();
}
