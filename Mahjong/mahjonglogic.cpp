#include "mahjonglogic.h"
#include <algorithm>
#include <random>
#include <chrono>

std::vector<int> MahjongLogic::tilesToCounts(const std::vector<Tile>& tiles) {
    std::vector<int> counts(30, 0);
    for (const auto& t : tiles) {
        counts[static_cast<int>(t.suit) * 10 + t.value]++;
    }
    return counts;
}

bool MahjongLogic::isWinningHand(std::vector<int> counts, bool hasPair) {
    int i = 0;
    while (i < 30 && counts[i] == 0) i++;
    if (i == 30) return true; // 所有牌都凑成了组合，胡牌

    // 尝试组对子
    if (!hasPair && counts[i] >= 2) {
        counts[i] -= 2;
        if (isWinningHand(counts, true)) return true;
        counts[i] += 2;
    }

    // 尝试组刻子 (三张相同)
    if (counts[i] >= 3) {
        counts[i] -= 3;
        if (isWinningHand(counts, hasPair)) return true;
        counts[i] += 3;
    }

    // 尝试组顺子 (必须是同花色，不能跨越8、9到下个花色)
    if (i % 10 <= 7 && i < 28 && counts[i + 1] > 0 && counts[i + 2] > 0) {
        counts[i]--;
        counts[i + 1]--;
        counts[i + 2]--;
        if (isWinningHand(counts, hasPair)) return true;
        counts[i]++;
        counts[i + 1]++;
        counts[i + 2]++;
    }

    return false;
}

std::vector<Tile> MahjongLogic::getWaits(const std::vector<Tile>& hand13) {
    std::vector<int> counts = tilesToCounts(hand13);
    std::vector<Tile> waits;

    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 9; ++v) {
            int idx = s * 10 + v;
            if (counts[idx] >= 4) continue; // 不能超过4张

            counts[idx]++;
            if (isWinningHand(counts, false)) {
                waits.push_back({static_cast<TileSuit>(s), v});
            }
            counts[idx]--;
        }
    }
    return waits;
}

std::map<Tile, std::vector<Tile>> MahjongLogic::getDiscardSuggestions(const std::vector<Tile>& hand) {
    std::map<Tile, std::vector<Tile>> suggestions;
    
    // 去重，避免同一种牌重复计算
    std::vector<Tile> uniqueHand = hand;
    uniqueHand.erase(std::unique(uniqueHand.begin(), uniqueHand.end()), uniqueHand.end());

    for (const auto& discardTile : uniqueHand) {
        std::vector<Tile> hand13 = hand;
        auto it = std::find(hand13.begin(), hand13.end(), discardTile);
        if (it != hand13.end()) {
            hand13.erase(it);
        }
        
        std::vector<Tile> waits = getWaits(hand13);
        if (!waits.empty()) {
            suggestions[discardTile] = waits;
        }
    }
    
    return suggestions;
}

std::vector<Tile> MahjongLogic::generateTrainingHand() {
    // 粗略生成一个处于“一向听”或已听牌状态的14张牌，用于训练
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    
    while (true) {
        std::vector<int> counts(30, 0);
        bool valid = true;
        
        auto addMeld = [&]() {
            if (rng() % 2 == 0) { // 刻子
                int s = rng() % 3; int v = 1 + rng() % 9;
                counts[s * 10 + v] += 3;
                if (counts[s * 10 + v] > 4) valid = false;
            } else { // 顺子
                int s = rng() % 3; int v = 1 + rng() % 7;
                counts[s * 10 + v]++; counts[s * 10 + v + 1]++; counts[s * 10 + v + 2]++;
                if (counts[s * 10 + v] > 4 || counts[s * 10 + v + 1] > 4 || counts[s * 10 + v + 2] > 4) valid = false;
            }
        };

        // 随机一对将牌
        int s = rng() % 3; int v = 1 + rng() % 9;
        counts[s * 10 + v] += 2;
        
        // 随机4副牌
        addMeld(); addMeld(); addMeld(); addMeld();
        
        if (!valid) continue; // 重新生成超过4张的非法牌型
        
        std::vector<Tile> hand;
        for (int i = 0; i < 30; ++i) {
            while (counts[i]-- > 0) {
                hand.push_back({static_cast<TileSuit>(i / 10), i % 10});
            }
        }
        
        // 从成型的14张牌中，随机抽取1张替换为任意其他牌（制造听牌局面）
        int removeIdx = rng() % 14;
        hand.erase(hand.begin() + removeIdx);
        hand.push_back({static_cast<TileSuit>(rng() % 3), 1 + (int)(rng() % 9)});
        
        std::sort(hand.begin(), hand.end());
        
        // 如果这副牌打出一张后存在听牌，则符合题意
        if (!getDiscardSuggestions(hand).empty()) {
            return hand;
        }
    }
}
