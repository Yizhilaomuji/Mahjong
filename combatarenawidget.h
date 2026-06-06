#ifndef COMBATARENAWIDGET_H
#define COMBATARENAWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>
#include <QGroupBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDir>
#include <QStandardPaths>
#include <vector>
#include "mahjonglogic.h"
#include "gamengine.h"
#include "aiplayer.h"
#include "videocapture.h"

// 模式枚举
enum class ArenaMode {
    RealtimeAssist,  // 实时辅助决策 (有抓包功能)
    AISimulation     // AI模拟训练 (无抓包功能)
};

// 对手画像信息
struct OpponentProfile {
    QString seatName;        // "上家"/"下家"/"对家"
    QString styleLabel;      // "进攻型"/"防守型"/"平衡型"
    QString dangerTiles;     // 高危炮牌列表
    int dangerProbability;   // 点炮概率 0-100
};

class CombatArenaWidget : public QWidget {
    Q_OBJECT
public:
    explicit CombatArenaWidget(ArenaMode mode, QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    // 游戏控制
    void onNewGame();
    void onDingqueClicked(TileSuit suit);
    void onHandTileClicked(int index);
    void onPongClicked();
    void onKongClicked();
    void onSelfKongClicked();
    void onWinClicked();
    void onPassClicked();

    // 对手手牌窥探
    void onPeekOpponent(int player);

    // AI 定时器
    void onAITimerTick();

    // 手动抓包
    void onManualCapture();
    void onUpdateGameState();
    void onStartVideoCapture();
    void onStartVoiceCapture();
    void onVoiceCommandSubmit();

    // 游戏引擎信号
    void onGameStarted();
    void onTurnChanged(int player);
    void onPlayerDiscarded(int player, Tile tile);
    void onPlayerPonged(int player, Tile tile);
    void onPlayerKonged(int player, Tile tile, GameEngine::KongType kongType);
    void onPlayerWon(int player, bool selfDrawn, int fan);
    void onDingqueSet(int player, TileSuit suit);
    void onGameOver();
    void onLogMessage(const QString &msg);

private:
    ArenaMode m_mode;
    GameEngine *m_engine;
    AIPlayer *m_ai;
    QTimer *m_aiTimer;

    // ===== UI 组件 =====
    QSplitter *m_splitter;

    // 左侧: 游戏面板
    QWidget *m_gamePanel;
    // 抓包选择栏 (仅实时模式)
    QComboBox *m_captureCombo;
    QPushButton *m_captureBtn;
    QPushButton *m_updateBtn;
    QWidget *m_captureBar;
    // 视频抓包
    VideoCaptureWidget *m_videoCapture;
    // 语音抓包
    QWidget *m_voicePanel;
    QLineEdit *m_voiceInput;
    QPushButton *m_voiceSubmitBtn;
    // 手动/语音模式回合追踪 (0=你, 1=下家, 2=对家, 3=上家)
    int m_manualTurnPlayer = 0;
    // 对手区域
    QGroupBox *m_opponentBoxes[4];  // [0]=自己, [1]=下家, [2]=对家, [3]=上家
    QLabel *m_oppDiscardLabels[4];
    QLabel *m_oppMeldLabels[4];
    QLabel *m_oppDingqueLabels[4];
    QScrollArea *m_oppHandScrolls[4];
    QWidget *m_oppHandInners[4];
    QHBoxLayout *m_oppHandInnerLayouts[4];
    // 自家面子展示
    QLabel *m_selfMeldLabel;
    // 中央信息
    QLabel *m_lastDiscardLabel;
    QLabel *m_statusLabel;
    QLabel *m_wallLabel;
    // 手牌区域
    QHBoxLayout *m_handLayout;
    QList<QPushButton*> m_handButtons;
    std::vector<Tile> m_displayHand;
    // 操作按钮
    QWidget *m_actionPanel;
    QPushButton *m_pongBtn;
    QPushButton *m_kongBtn;
    QPushButton *m_selfKongBtn;
    QPushButton *m_winBtn;
    QPushButton *m_passBtn;
    // 定缺面板
    QWidget *m_dingquePanel;
    // 日志
    QTextEdit *m_logView;

    // 右侧: 侧边栏
    QWidget *m_sidebar;
    // 出牌建议
    QGroupBox *m_suggestGroup;
    QTextEdit *m_suggestionBox;
    // 对手画像
    QGroupBox *m_profileGroup;
    QTextEdit *m_profileBox[3];  // 上家/下家/对家
    QLabel *m_profileStyleLabel[3];
    QLabel *m_profileDangerLabel[3];

    // 背景音乐
    QMediaPlayer *m_bgmPlayer = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    // ===== 内部方法 =====
    void setupUI();
    void setupGamePanel(QVBoxLayout *mainLayout);
    void setupOpponentAreas(QGridLayout *grid);
    void setupSidebar(QVBoxLayout *sidebarLayout);
    QGroupBox *createOpponentBox(int seatIndex);
    void refreshAllUI();
    void refreshOpponentUI(int seatIndex);
    void refreshSelfMeld();
    void refreshHandUI();
    void refreshActionButtons();
    void refreshSuggestions();
    void refreshOpponentProfiles();
    void showDingquePanel();
    void hideDingquePanel();
    void initBackgroundMusic();

    QString tileToDisplay(Tile t) const;
    QString suitColor(TileSuit suit) const;

    bool isHumanTurn() const;
    void runAITurn();
    int mapSeatToPlayer(int seatIndex) const; // seat -> engine player index
    int mapPlayerToSeat(int player) const;    // engine player index -> seat
};

#endif // COMBATARENAWIDGET_H
