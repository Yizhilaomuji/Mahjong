#include "gamengine.h"
#include <algorithm>

GameEngine::GameEngine(QObject *parent) : QObject(parent) {
    m_players[0].isHuman = true;
}

void GameEngine::startNewGame() {
    m_wall = MahjongLogic::generateWall();
    for (int i = 0; i < 4; ++i) {
        m_players[i] = PlayerState();
        m_players[i].isHuman = (i == 0);
    }
    m_phase = Dealing;
    m_currentPlayer = 0;
    m_lastDiscardPlayer = -1;
    m_lastActionWasDiscard = false;

    dealTiles();
    m_phase = Dingque;
    emit gameStarted();
    emit phaseChanged(m_phase);
}

void GameEngine::injectRealtimeGame(const std::vector<Tile>& hand, TileSuit dingque) {
    // 不触发正常发牌流程，直接注入牌局状态
    m_wall = MahjongLogic::generateWall();
    for (int i = 0; i < 4; ++i) {
        m_players[i] = PlayerState();
        m_players[i].isHuman = (i == 0);
    }
    m_currentPlayer = 0;
    m_lastDiscardPlayer = -1;
    m_lastActionWasDiscard = false;

    // 注入人类玩家的手牌和定缺
    m_players[0].hand = hand;
    std::sort(m_players[0].hand.begin(), m_players[0].hand.end());
    m_players[0].dingque = dingque;
    m_players[0].dingqueSet = true;
    checkDingqueDone(0);

    // 对手生成随机手牌（仅用于 UI 显示数量），不参与实际对局
    for (int i = 1; i < 4; ++i) {
        for (int j = 0; j < 13; ++j) {
            m_players[i].hand.push_back(m_wall.back());
            m_wall.pop_back();
        }
        TileSuit dq = dingque == TileSuit::Dot ? TileSuit::Bamboo
            : (dingque == TileSuit::Bamboo ? TileSuit::Character : TileSuit::Dot);
        m_players[i].dingque = dq;
        m_players[i].dingqueSet = true;
    }

    m_phase = Playing;
    emit gameStarted();
    emit phaseChanged(m_phase);
    emit logMessage("===== 牌局已注入 (实战模式) =====");
}

void GameEngine::dealTiles() {
    int wallIdx = 0;
    for (int p = 0; p < 4; ++p) {
        for (int i = 0; i < 13; ++i) {
            m_players[p].hand.push_back(m_wall[wallIdx++]);
        }
    }
    // 庄家多一张
    m_players[0].hand.push_back(m_wall[wallIdx++]);
    m_wall.erase(m_wall.begin(), m_wall.begin() + wallIdx);

    for (int p = 0; p < 4; ++p) {
        sortHand(p);
    }
}

void GameEngine::setDingque(int player, TileSuit suit) {
    m_players[player].dingque = suit;
    m_players[player].dingqueSet = true;
    emit dingqueSet(player, suit);
    emit logMessage(QString("玩家%1 定缺: %2")
        .arg(player + 1)
        .arg(suit == TileSuit::Dot ? "缺筒" : (suit == TileSuit::Bamboo ? "缺条" : "缺万")));

    // 检查是否所有人都定了缺
    for (int i = 0; i < 4; ++i) {
        if (!m_players[i].dingqueSet) return;
    }

    m_phase = Playing;
    emit phaseChanged(m_phase);
    emit logMessage("===== 定缺完毕，对局开始 (庄家先手) =====");
    // 庄家先摸牌
    playerDraw();
}

void GameEngine::setHuman(int player, bool human) {
    m_players[player].isHuman = human;
}

int GameEngine::aliveCount() const {
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (!m_players[i].hasWon) count++;
    }
    return count;
}

bool GameEngine::canPong(int player) const {
    if (player == m_lastDiscardPlayer) return false;
    if (m_players[player].hasWon) return false;
    if (!m_lastActionWasDiscard) return false;
    if (m_players[player].dingque == m_lastDiscard.suit && !m_players[player].dingqueDone)
        return false; // 缺门不能碰

    int count = 0;
    for (const auto &t : m_players[player].hand) {
        if (t == m_lastDiscard) count++;
    }
    return count >= 2;
}

bool GameEngine::canExposedKong(int player) const {
    if (player == m_lastDiscardPlayer) return false;
    if (m_players[player].hasWon) return false;
    if (!m_lastActionWasDiscard) return false;

    int count = 0;
    for (const auto &t : m_players[player].hand) {
        if (t == m_lastDiscard) count++;
    }
    return count >= 3;
}

bool GameEngine::canSelfKong(int player) const {
    // 只有当前回合玩家可以暗杠/加杠
    if (player != m_currentPlayer) return false;
    if (m_players[player].hasWon) return false;
    if (m_lastActionWasDiscard) return false; // 不是在响应出牌

    // 检查暗杠：手牌中有4张相同的
    std::vector<int> counts = MahjongLogic::tilesToCounts(m_players[player].hand);
    for (int i = 0; i < 30; ++i) {
        if (counts[i] >= 4) return true;
    }

    // 检查加杠：已碰的牌中，手牌又摸到了第4张
    for (const auto &meld : m_players[player].melds) {
        if (meld.size() == 3) {
            // 这是碰的牌组
            Tile meldTile = meld[0];
            for (const auto &t : m_players[player].hand) {
                if (t == meldTile) return true;
            }
        }
    }
    return false;
}

bool GameEngine::canWinOnDiscard(int player) const {
    if (player == m_lastDiscardPlayer) return false;
    if (m_players[player].hasWon) return false;
    if (!m_lastActionWasDiscard) return false;
    if (m_players[player].dingqueSet && !m_players[player].dingqueDone) return false;

    std::vector<Tile> hand14 = m_players[player].hand;
    hand14.push_back(m_lastDiscard);
    std::sort(hand14.begin(), hand14.end());
    return MahjongLogic::canWin(hand14);
}

bool GameEngine::canSelfWin(int player) const {
    if (player != m_currentPlayer) return false;
    if (m_players[player].hasWon) return false;
    if (m_lastActionWasDiscard) return false;
    if (m_players[player].dingqueSet && !m_players[player].dingqueDone) return false;

    return MahjongLogic::canWin(m_players[player].hand);
}

void GameEngine::playerDraw() {
    if (m_phase != Playing) return;
    if (m_wall.empty()) {
        m_phase = Finished;
        emit phaseChanged(m_phase);
        emit gameOver();
        return;
    }

    Tile drawn = drawFromWall();
    m_players[m_currentPlayer].hand.push_back(drawn);
    sortHand(m_currentPlayer);
    m_lastActionWasDiscard = false;
    checkDingqueDone(m_currentPlayer);

    emit playerDrew(m_currentPlayer);
    emit logMessage(QString("玩家%1 摸牌").arg(m_currentPlayer + 1));

    // 摸牌后检查可选动作
    checkAutoActions();
}

Tile GameEngine::drawFromWall() {
    Tile t = m_wall.back();
    m_wall.pop_back();
    return t;
}

void GameEngine::playerDiscard(Tile tile) {
    if (m_phase != Playing) return;
    if (m_currentPlayer < 0 || m_currentPlayer >= 4) return;

    // 定缺规则: 缺门未打完时只能打缺门花色
    const PlayerState &ps = m_players[m_currentPlayer];
    if (ps.dingqueSet && !ps.dingqueDone && tile.suit != ps.dingque) {
        QString suitName = ps.dingque == TileSuit::Dot ? "筒" : (ps.dingque == TileSuit::Bamboo ? "条" : "万");
        emit logMessage(QString("玩家%1 必须优先打出缺门花色(%2)!")
            .arg(m_currentPlayer + 1).arg(suitName));
        return;
    }

    bool removed = MahjongLogic::removeTile(m_players[m_currentPlayer].hand, tile);
    if (!removed) return;

    m_players[m_currentPlayer].discards.push_back(tile);
    m_lastDiscard = tile;
    m_lastDiscardPlayer = m_currentPlayer;
    m_lastActionWasDiscard = true;
    checkDingqueDone(m_currentPlayer);

    emit playerDiscarded(m_currentPlayer, tile);
    emit logMessage(QString("玩家%1 打出 %2").arg(m_currentPlayer + 1).arg(tile.toString()));

    // 检查其他玩家是否可以响应
    checkResponseActions();
}

void GameEngine::playerPong(int player) {
    if (!canPong(player)) return;

    // 从手牌中移除2张与 lastDiscard 相同的牌
    std::vector<Tile> pongTiles;
    pongTiles.push_back(m_lastDiscard);
    int removed = 0;
    auto it = m_players[player].hand.begin();
    while (it != m_players[player].hand.end() && removed < 2) {
        if (*it == m_lastDiscard) {
            pongTiles.push_back(*it);
            it = m_players[player].hand.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    m_players[player].melds.push_back(pongTiles);
    sortHand(player);
    m_lastActionWasDiscard = false;

    emit playerPonged(player, m_lastDiscard);
    emit logMessage(QString("玩家%1 碰 %2").arg(player + 1).arg(m_lastDiscard.toString()));

    // 碰牌后轮到碰牌者出牌
    m_currentPlayer = player;
    emit turnChanged(m_currentPlayer);
}

void GameEngine::playerKong(int player) {
    // 明杠：碰别人打出的牌（手牌有3张）
    if (!canExposedKong(player)) return;

    std::vector<Tile> kongTiles;
    kongTiles.push_back(m_lastDiscard);
    int removed = 0;
    auto it = m_players[player].hand.begin();
    while (it != m_players[player].hand.end() && removed < 3) {
        if (*it == m_lastDiscard) {
            kongTiles.push_back(*it);
            it = m_players[player].hand.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    m_players[player].melds.push_back(kongTiles);
    sortHand(player);
    m_lastActionWasDiscard = false;

    // 杠后补牌
    if (!m_wall.empty()) {
        Tile extra = drawFromWall();
        m_players[player].hand.push_back(extra);
        sortHand(player);
    }

    emit playerKonged(player, m_lastDiscard, ExposedKong);
    emit logMessage(QString("玩家%1 杠 %2 (明杠)").arg(player + 1).arg(m_lastDiscard.toString()));

    m_currentPlayer = player;
    emit turnChanged(m_currentPlayer);
}

void GameEngine::playerSelfKong(int player, Tile tile, KongType kongType) {
    if (player != m_currentPlayer) return;
    if (m_players[player].hasWon) return;
    if (m_lastActionWasDiscard) return; // 必须在摸牌后才能暗杠/加杠

    if (kongType == HiddenKong) {
        // 暗杠：手牌中移除4张
        std::vector<Tile> kongTiles;
        int removed = 0;
        auto it = m_players[player].hand.begin();
        while (it != m_players[player].hand.end() && removed < 4) {
            if (*it == tile) {
                kongTiles.push_back(*it);
                it = m_players[player].hand.erase(it);
                removed++;
            } else {
                ++it;
            }
        }
        if (removed < 4) return;
        m_players[player].melds.push_back(kongTiles);
    } else if (kongType == AddKong) {
        // 加杠：找到已碰的meld，加入手牌中的第4张
        bool foundMeld = false;
        for (auto &meld : m_players[player].melds) {
            if (meld.size() == 3 && meld[0] == tile) {
                meld.push_back(tile);
                foundMeld = true;
                break;
            }
        }
        if (!foundMeld) return;
        MahjongLogic::removeTile(m_players[player].hand, tile);
    }

    sortHand(player);

    // 杠后补牌
    if (!m_wall.empty()) {
        Tile extra = drawFromWall();
        m_players[player].hand.push_back(extra);
        sortHand(player);
    }

    emit playerKonged(player, tile, kongType);
    QString kongName = kongType == HiddenKong ? "暗杠" : "加杠";
    emit logMessage(QString("玩家%1 %2 %3").arg(player + 1).arg(kongName).arg(tile.toString()));

    // 杠后不改变 currentPlayer，仍由该玩家出牌
    emit turnChanged(m_currentPlayer);
}

void GameEngine::playerWinOnDiscard(int player) {
    if (!canWinOnDiscard(player)) return;

    m_players[player].hand.push_back(m_lastDiscard);
    sortHand(player);
    m_players[player].hasWon = true;
    int fan = MahjongLogic::calculateFanTypes(m_players[player].hand);

    m_lastActionWasDiscard = false;
    emit playerWon(player, false, fan);
    emit logMessage(QString("玩家%1 胡! (点炮, %2番)").arg(player + 1).arg(fan));

    checkGameEnd();
}

void GameEngine::playerSelfWin(int player) {
    if (!canSelfWin(player)) return;

    m_players[player].hasWon = true;
    int fan = MahjongLogic::calculateFanTypes(m_players[player].hand);

    m_lastActionWasDiscard = false;
    emit playerWon(player, true, fan);
    emit logMessage(QString("玩家%1 自摸! (%2番)").arg(player + 1).arg(fan));

    checkGameEnd();
}

void GameEngine::advanceTurn() {
    if (m_phase != Playing) return;
    m_currentPlayer = nextAlivePlayer(m_currentPlayer);
    emit turnChanged(m_currentPlayer);
    playerDraw();
}

int GameEngine::nextAlivePlayer(int from) const {
    for (int i = 1; i <= 3; ++i) {
        int next = (from + i) % 4;
        if (!m_players[next].hasWon) return next;
    }
    return from; // 理论上不会到这里
}

void GameEngine::checkAutoActions() {
    // 摸牌后：检查自摸和暗杠/加杠
    // 对于非人类玩家，AI 会自动决策；对于人类玩家，UI 提供按钮
    // 这里只发射信号，由 UI/AI 层决定
    emit turnChanged(m_currentPlayer);
}

void GameEngine::checkResponseActions() {
    // 出牌后：按优先级 胡 > 杠 > 碰 检查
    // 对于非人类玩家，AI 自动决策；对于人类玩家，UI 提供按钮
    // 如果没有任何人可以响应，自动推进回合
    bool anyResponse = false;
    for (int i = 0; i < 4; ++i) {
        if (i == m_lastDiscardPlayer) continue;
        if (m_players[i].hasWon) continue;

        if (canWinOnDiscard(i)) { anyResponse = true; break; }
        if (canExposedKong(i)) { anyResponse = true; break; }
        if (canPong(i)) { anyResponse = true; break; }
    }

    if (!anyResponse) {
        // 无人可响应，推进到下一人
        emit logMessage("(无人响应，下一家摸牌)");
        advanceTurn();
    }
    // 有人可响应时，由 AI 层或 UI 层处理
}

void GameEngine::checkGameEnd() {
    if (aliveCount() <= 1 || m_wall.empty()) {
        m_phase = Finished;
        emit phaseChanged(m_phase);
        emit gameOver();

        // 找出未胡的玩家
        for (int i = 0; i < 4; ++i) {
            if (!m_players[i].hasWon) {
                emit logMessage(QString("玩家%1 流局").arg(i + 1));
            }
        }
        emit logMessage("===== 对局结束 =====");
        return;
    }

    // 血战到底：有人胡牌后推进到下一个存活玩家
    // 可能胡牌的是当前玩家(自摸)、弃牌者、或任意响应弃牌的玩家
    m_currentPlayer = nextAlivePlayer(m_lastDiscardPlayer >= 0 ? m_lastDiscardPlayer : m_currentPlayer);
    playerDraw();
}

void GameEngine::sortHand(int player) {
    std::sort(m_players[player].hand.begin(), m_players[player].hand.end());
}

void GameEngine::checkDingqueDone(int player) {
    bool hasDingqueSuit = false;
    for (const auto &t : m_players[player].hand) {
        if (t.suit == m_players[player].dingque) {
            hasDingqueSuit = true;
            break;
        }
    }
    bool wasDone = m_players[player].dingqueDone;
    m_players[player].dingqueDone = !hasDingqueSuit;
    if (!wasDone && m_players[player].dingqueDone) {
        emit logMessage(QString("玩家%1 缺门已打完").arg(player + 1));
    }
}
