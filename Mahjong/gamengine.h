#ifndef GAMENGINE_H
#define GAMENGINE_H

#include <QObject>
#include <vector>
#include "mahjonglogic.h"

struct PlayerState {
    std::vector<Tile> hand;
    std::vector<std::vector<Tile>> melds; // 碰/杠的组合
    std::vector<Tile> discards;
    TileSuit dingque = TileSuit::Dot;
    bool dingqueSet = false;   // 是否已定缺
    bool dingqueDone = false;  // 缺门已打完
    bool hasWon = false;
    bool isHuman = false;
};

class GameEngine : public QObject {
    Q_OBJECT
public:
    enum Phase { Idle, Dealing, Dingque, Playing, Finished };
    // 杠的类型
    enum KongType { ExposedKong = 0, HiddenKong = 1, AddKong = 2 };

    explicit GameEngine(QObject *parent = nullptr);

    void startNewGame();
    void injectRealtimeGame(const std::vector<Tile>& hand, TileSuit dingque);
    void setDingque(int player, TileSuit suit);
    void setHuman(int player, bool human);

    // 查询
    const PlayerState &playerState(int index) const { return m_players[index]; }
    int currentPlayer() const { return m_currentPlayer; }
    Phase phase() const { return m_phase; }
    Tile lastDiscard() const { return m_lastDiscard; }
    int lastDiscardPlayer() const { return m_lastDiscardPlayer; }
    int tilesRemaining() const { return static_cast<int>(m_wall.size()); }
    int aliveCount() const; // 未胡人数

    // 动作合法性检查
    bool canPong(int player) const;
    bool canExposedKong(int player) const;
    bool canSelfKong(int player) const;  // 暗杠或加杠
    bool canWinOnDiscard(int player) const;
    bool canSelfWin(int player) const;   // 自摸

    // 动作执行
    void playerDraw();
    void playerDiscard(Tile tile);
    void playerPong(int player);
    void playerKong(int player);         // 碰杠(明杠)
    void playerSelfKong(int player, Tile tile, KongType kongType);
    void playerWinOnDiscard(int player);
    void playerSelfWin(int player);

    // 推进到下一个可行动的玩家
    void advanceTurn();

    // 检查定缺是否完成 (外部更新牌局后调用)
    void checkDingqueDone(int player);

signals:
    void gameStarted();
    void phaseChanged(GameEngine::Phase phase);
    void turnChanged(int player);
    void playerDrew(int player);
    void playerDiscarded(int player, Tile tile);
    void playerPonged(int player, Tile tile);
    void playerKonged(int player, Tile tile, GameEngine::KongType kongType);
    void playerWon(int player, bool selfDrawn, int fan);
    void dingqueSet(int player, TileSuit suit);
    void gameOver();
    void logMessage(const QString &msg);

private:
    std::vector<Tile> m_wall;
    PlayerState m_players[4];
    Phase m_phase = Idle;
    int m_currentPlayer = 0;
    Tile m_lastDiscard;
    int m_lastDiscardPlayer = -1;
    bool m_lastActionWasDiscard = false; // 最近一次动作是否是出牌（用于判定响应窗口）

    void dealTiles();
    Tile drawFromWall();
    int nextAlivePlayer(int from) const;
    void checkAutoActions(); // 摸牌后检查暗杠/自摸等
    void checkResponseActions(); // 出牌后检查碰/杠/胡
    void checkGameEnd();
    void sortHand(int player);
};

#endif // GAMENGINE_H
