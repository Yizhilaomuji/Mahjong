#include "aiplayer.h"
#include <algorithm>
#include <map>
#include <set>

AIPlayer::AIPlayer(QObject *parent) : QObject(parent) {}

TileSuit AIPlayer::chooseDingque(const std::vector<Tile> &hand) {
    int counts[3] = {0, 0, 0};
    for (const auto &t : hand) {
        counts[static_cast<int>(t.suit)]++;
    }
    // 选择张数最少的花色
    int minIdx = 0;
    if (counts[1] < counts[minIdx]) minIdx = 1;
    if (counts[2] < counts[minIdx]) minIdx = 2;
    return static_cast<TileSuit>(minIdx);
}

Tile AIPlayer::chooseDiscard(const std::vector<Tile> &hand,
                            TileSuit dingque,
                            const GameContext &ctx) {
    // 优先级1: 如果有定缺花色的牌，优先打出
    for (const auto &t : hand) {
        if (t.suit == dingque) return t;
    }

    // 收集对手已打出的牌（熟张）
    std::set<Tile> safeTiles;
    for (int i = 0; i < 4; ++i) {
        for (const auto &d : ctx.discards[i]) {
            safeTiles.insert(d);
        }
    }

    // 优先级2: 孤立牌（同花色内无相邻牌）
    std::vector<Tile> isolated;
    std::vector<Tile> nonIsolated;
    for (const auto &t : hand) {
        if (MahjongLogic::isIsolated(hand, t)) {
            isolated.push_back(t);
        } else {
            nonIsolated.push_back(t);
        }
    }

    // 优先从孤立牌中选择熟张
    for (const auto &t : isolated) {
        if (safeTiles.count(t) > 0) return t;
    }

    // 如果有孤立牌，优先打出
    if (!isolated.empty()) return isolated[0];

    // 优先级3: 从非孤立牌中计算听牌数，打出听牌数最多的
    int bestWaits = -1;
    Tile bestDiscard = hand[0];
    for (const auto &t : nonIsolated) {
        int waits = countWaitsAfterDiscard(hand, t);
        if (waits > bestWaits) {
            bestWaits = waits;
            bestDiscard = t;
        } else if (waits == bestWaits && safeTiles.count(t) > 0) {
            // 同等听牌数下优先熟张
            bestDiscard = t;
        }
    }

    return bestDiscard;
}

bool AIPlayer::shouldPong(Tile tile, const std::vector<Tile> &hand,
                           TileSuit dingque) {
    // 若碰的是缺门，不碰（通常不会，因为 canPong 已有检查）
    if (tile.suit == dingque) return false;

    // 简单策略：碰了之后能减少向听数就碰
    std::vector<Tile> afterPong = hand;
    int removed = 0;
    auto it = afterPong.begin();
    while (it != afterPong.end() && removed < 2) {
        if (*it == tile) {
            it = afterPong.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    // 碰牌后手牌减少2张，通常会让牌型更好
    // 简单判断：碰牌后如果不是最后几张牌，就碰
    return afterPong.size() >= 4;
}

bool AIPlayer::shouldKong(Tile tile, const std::vector<Tile> &hand,
                           TileSuit dingque) {
    // 杠会增加番数，但破坏手牌结构
    // 简单策略：已听牌时不杠（避免破坏听牌），未听牌时杠
    if (tile.suit == dingque) return false;

    // 检查当前是否已听牌
    for (const auto &t : hand) {
        if (countWaitsAfterDiscard(hand, t) > 0) return false; // 已听牌不杠
    }
    return true;
}

bool AIPlayer::shouldSelfKong(const std::vector<Tile> &hand,
                               const std::vector<std::vector<Tile>> &melds) {
    // 已听牌时不杠
    for (const auto &t : hand) {
        if (countWaitsAfterDiscard(hand, t) > 0) return false;
    }

    // 暗杠机会：手牌中有4张相同
    std::vector<int> counts = MahjongLogic::tilesToCounts(hand);
    for (int i = 0; i < 30; ++i) {
        if (counts[i] >= 4) return true;
    }

    // 加杠机会：手牌中有已碰的第4张
    for (const auto &meld : melds) {
        if (meld.size() == 3) {
            for (const auto &t : hand) {
                if (t == meld[0]) return true;
            }
        }
    }
    return false;
}

int AIPlayer::countWaitsAfterDiscard(const std::vector<Tile> &hand,
                                      Tile discard) {
    return MahjongLogic::countRemainingWaitTilesAfterDiscard(hand, discard);
}
