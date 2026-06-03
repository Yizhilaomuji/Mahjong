#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QObject>
#include <vector>
#include "mahjonglogic.h"

struct GameContext {
    std::vector<Tile> discards[4];   // 各玩家的弃牌堆
    std::vector<std::vector<Tile>> melds[4]; // 各玩家的碰/杠
    int aliveCount;
};

class AIPlayer : public QObject {
    Q_OBJECT
public:
    explicit AIPlayer(QObject *parent = nullptr);

    // 选择定缺：手牌最少的花色
    TileSuit chooseDingque(const std::vector<Tile> &hand);

    // 选择打出的牌
    Tile chooseDiscard(const std::vector<Tile> &hand, TileSuit dingque,
                       const GameContext &ctx);

    // 是否碰
    bool shouldPong(Tile tile, const std::vector<Tile> &hand,
                    TileSuit dingque);

    // 是否杠（明杠）
    bool shouldKong(Tile tile, const std::vector<Tile> &hand,
                    TileSuit dingque);

    // 是否暗杠/加杠
    bool shouldSelfKong(const std::vector<Tile> &hand,
                        const std::vector<std::vector<Tile>> &melds);

    // 是否胡（总是胡）
    bool shouldWin() { return true; }

private:
    // 计算打出一张牌能听多少种牌
    int countWaitsAfterDiscard(const std::vector<Tile> &hand, Tile discard);
};

#endif // AIPLAYER_H
