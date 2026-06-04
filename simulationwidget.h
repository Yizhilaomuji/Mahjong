#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <vector>
#include "mahjonglogic.h"
#include "gamengine.h"
#include "aiplayer.h"

class SimulationWidget : public QWidget {
    Q_OBJECT
public:
    explicit SimulationWidget(QWidget *parent = nullptr);

private slots:
    void onNewGame();
    void onDingqueClicked(TileSuit suit);
    void onHandTileClicked(int index);
    void onPongClicked();
    void onKongClicked();
    void onSelfKongClicked();
    void onWinClicked();
    void onPassClicked();
    void onAutoPilotToggled(int player, bool enabled);
    void onAITimerTick();

    // Game engine signal handlers
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
    GameEngine *m_engine;
    AIPlayer *m_ai;
    QTimer *m_aiTimer;

    // 托管 checkbox
    QCheckBox *m_autoPilot[4];

    // 顶部信息
    QLabel *m_wallLabel;
    QLabel *m_statusLabel;
    QLabel *m_lastDiscardLabel;

    // 玩家区域
    QGroupBox *m_playerBoxes[4];
    QHBoxLayout *m_handLayouts[4];
    QLabel *m_meldLabels[4];
    QLabel *m_discardLabels[4];
    QLabel *m_dingqueLabels[4];
    QLabel *m_statusLabels[4];
    QList<QPushButton*> m_handButtons[4];

    // 手牌平铺显示
    std::vector<Tile> m_displayHands[4];

    // 操作按钮区
    QWidget *m_actionPanel;
    QPushButton *m_pongBtn;
    QPushButton *m_kongBtn;
    QPushButton *m_selfKongBtn;
    QPushButton *m_winBtn;
    QPushButton *m_passBtn;

    // 定缺选择区
    QWidget *m_dingquePanel;
    QPushButton *m_dingqueDotBtn;
    QPushButton *m_dingqueBambooBtn;
    QPushButton *m_dingqueCharBtn;

    // 日志
    QTextEdit *m_logView;

    void setupUI();
    QGroupBox *createPlayerBox(int index);
    void refreshAllPlayerUI();
    void refreshPlayerUI(int player);
    void refreshActionButtons();
    void showDingquePanel();
    void hideDingquePanel();
    QString tileToDisplay(Tile t) const;
    QString suitColor(TileSuit suit) const;

    bool isHumanTurn() const;
    void runAITurn();
};

#endif // SIMULATIONWIDGET_H
