#include "mahjonglogic.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include <climits>
#include <QRegularExpression>
#include <QStringList>

int MahjongLogic::calculateFanTypes(const std::vector<Tile>& hand14) {
    if (hand14.size() != 14) return 0;
    
    int fan = 1; // 基础平胡 1番
    
    // 检查清一色
    bool qingyise = true;
    TileSuit firstSuit = hand14[0].suit;
    for (const auto& t : hand14) {
        if (t.suit != firstSuit) {
            qingyise = false;
            break;
        }
    }
    if (qingyise) fan *= 4; // 清一色 x4
    
    // 检查七对 (简单判断成双)
    bool qidui = true;
    auto sorted = hand14;
    std::sort(sorted.begin(), sorted.end());
    for (size_t i = 0; i < 14; i += 2) {
        if (sorted[i].suit != sorted[i+1].suit || sorted[i].value != sorted[i+1].value) {
            qidui = false; break;
        }
    }
    if (qidui) fan *= 4; // 七对 x4
    
    // TODO: 对对胡（碰碰胡）、带幺九等判断
    return fan;
}

std::vector<int> MahjongLogic::tilesToCounts(const std::vector<Tile>& tiles) {
    std::vector<int> counts(30, 0);
    for (const auto& t : tiles) {
        counts[static_cast<int>(t.suit) * 10 + t.value]++;
    }
    return counts;
}

int MahjongLogic::remainingCopies(const std::vector<Tile>& hand, Tile tile) {
    std::vector<int> counts = tilesToCounts(hand);
    int idx = static_cast<int>(tile.suit) * 10 + tile.value;
    int remain = 4 - counts[idx];
    return remain < 0 ? 0 : remain;
}

int MahjongLogic::countRemainingWaitTiles(const std::vector<Tile>& hand, const std::vector<Tile>& waits) {
    int total = 0;
    for (const auto& wait : waits) {
        total += remainingCopies(hand, wait);
    }
    return total;
}

int MahjongLogic::countRemainingWaitTilesAfterDiscard(const std::vector<Tile>& hand, Tile discard) {
    std::vector<Tile> hand13 = hand;
    removeTile(hand13, discard);
    auto waits = getWaits(hand13);
    return countRemainingWaitTiles(hand13, waits);
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

std::vector<std::pair<Tile, std::vector<Tile>>> MahjongLogic::getBestDiscardOptions(const std::vector<Tile>& hand) {
    auto suggestions = getDiscardSuggestions(hand);
    std::vector<std::pair<Tile, std::vector<Tile>>> bestOptions;
    int maxWaits = 0;
    for (const auto& pair : suggestions) {
        std::vector<Tile> afterDiscard = hand;
        removeTile(afterDiscard, pair.first);
        int waitCount = countRemainingWaitTiles(afterDiscard, pair.second);
        if (waitCount > maxWaits) {
            maxWaits = waitCount;
        }
    }
    for (const auto& pair : suggestions) {
        std::vector<Tile> afterDiscard = hand;
        removeTile(afterDiscard, pair.first);
        int waitCount = countRemainingWaitTiles(afterDiscard, pair.second);
        if (waitCount == maxWaits) {
            bestOptions.emplace_back(pair.first, pair.second);
        }
    }
    return bestOptions;
}

std::vector<Tile> MahjongLogic::generateTrainingHand() {
    // 粗略生成一个处于“一向听”或已听牌状态的14张牌，用于训练
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    
    while (true) {
        std::vector<int> counts(30, 0);
        bool valid = true;
        
        auto addMeld = [&]() {
            if (rng() % 2 == 0) { // 刻子
                int s = 2; int v = 1 + rng() % 9; // 仅万字
                counts[s * 10 + v] += 3;
                if (counts[s * 10 + v] > 4) valid = false;
            } else { // 顺子
                int s = 2; int v = 1 + rng() % 7; // 仅万字
                counts[s * 10 + v]++; counts[s * 10 + v + 1]++; counts[s * 10 + v + 2]++;
                if (counts[s * 10 + v] > 4 || counts[s * 10 + v + 1] > 4 || counts[s * 10 + v + 2] > 4) valid = false;
            }
        };

        // 随机一对将牌
        int s = 2; int v = 1 + rng() % 9; // 仅万字
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
        hand.push_back({TileSuit::Character, 1 + (int)(rng() % 9)}); // 替换为万字
        
        std::sort(hand.begin(), hand.end());
        
        // 如果这副牌打出一张后存在听牌，则符合题意
        if (!getDiscardSuggestions(hand).empty()) {
            return hand;
        }
    }
}

std::vector<Tile> MahjongLogic::generateWall() {
    std::vector<Tile> wall;
    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 9; ++v) {
            for (int i = 0; i < 4; ++i) {
                wall.push_back({static_cast<TileSuit>(s), v});
            }
        }
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(wall.begin(), wall.end(), std::mt19937(seed));
    return wall;
}

bool MahjongLogic::canWin(const std::vector<Tile>& hand14) {
    if (hand14.size() != 14) return false;
    std::vector<int> counts = tilesToCounts(hand14);
    return isWinningHand(counts, false);
}

int MahjongLogic::countSuit(const std::vector<Tile>& hand, TileSuit suit) {
    int count = 0;
    for (const auto& t : hand) {
        if (t.suit == suit) count++;
    }
    return count;
}

bool MahjongLogic::isIsolated(const std::vector<Tile>& hand, Tile tile) {
    for (const auto& t : hand) {
        if (t.suit != tile.suit) continue;
        if (t.value == tile.value) continue; // same tile, not neighbor
        int diff = std::abs(t.value - tile.value);
        if (diff <= 2) return false; // has a neighbor within ±2
    }
    return true;
}

bool MahjongLogic::hasTile(const std::vector<Tile>& hand, Tile tile) {
    for (const auto& t : hand) {
        if (t == tile) return true;
    }
    return false;
}

bool MahjongLogic::removeTile(std::vector<Tile>& hand, Tile tile) {
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (*it == tile) {
            hand.erase(it);
            return true;
        }
    }
    return false;
}

// ---- 向听数计算 ----

// 从 counts 中贪心提取完整面子（刻子+顺子）和搭子，不重复使用牌
// 返回 (melds, taatsu, pairs) — taatsu 不包含对子，pairs 是对子组数
static std::tuple<int, int, int>
countMeldsAndTaatsu(std::vector<int> counts) {
    int melds = 0;
    int taatsu = 0;

    // 1) 先提取完整刻子（优先级最高）
    for (int i = 1; i < 30; ++i) {
        if (counts[i] >= 3) {
            int n = counts[i] / 3;
            melds += n;
            counts[i] -= n * 3;
        }
    }

    // 2) 提取完整顺子
    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 7; ++v) {
            int i = s * 10 + v;
            while (counts[i] > 0 && counts[i + 1] > 0 && counts[i + 2] > 0) {
                melds++;
                counts[i]--;
                counts[i + 1]--;
                counts[i + 2]--;
            }
        }
    }

    // 3) 统计序列搭子：用 used 标记，每张牌最多参与一个搭子
    //    优先分配牌给搭子（两面/坎张），再给对子（单张等刻子）
    std::vector<bool> used(30, false);

    // 3a) 相邻搭子 (i, i+1) — 优先级最高
    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 8; ++v) {
            int i = s * 10 + v;
            if (counts[i] > 0 && counts[i + 1] > 0 && !used[i] && !used[i + 1]) {
                taatsu++;
                used[i] = true;
                used[i + 1] = true;
            }
        }
    }

    // 3b) 间隔搭子 (i, i+2)
    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 7; ++v) {
            int i = s * 10 + v;
            if (counts[i] > 0 && counts[i + 2] > 0 && !used[i] && !used[i + 2]) {
                taatsu++;
                used[i] = true;
                used[i + 2] = true;
            }
        }
    }

    // 4) 统计对子：仅从尚未被搭子占用的牌中统计
    //    一张牌被搭子占用后，即使用于对子还有剩余张数，也不再计为对子
    int pairs = 0;
    for (int i = 1; i < 30; ++i) {
        if (!used[i] && counts[i] >= 2) {
            pairs++;
            // 不标记 used，对子仅占用该位置的牌，不影响其他位置的搭子统计
        }
    }

    return {melds, taatsu, pairs};
}

// 计算向听数：标准公式
// shanten = 8 - 2*melds - min(taatsu + extra_pairs, 4-melds) - hasPair
// 对子的处理：至多 1 对保留为雀头，其余对子视为搭子（差一张成刻子）
static int computeShanten13(const std::vector<int>& counts) {
    auto [melds, taatsu, pairs] = countMeldsAndTaatsu(counts);

    int remainingMeldsNeeded = 4 - melds;

    // 对子中保留 1 对做雀头，其余视为搭子
    int pairsForTaatsu = (pairs > 0) ? pairs - 1 : 0;
    int totalTaatsu = taatsu + pairsForTaatsu;
    int effectiveTaatsu = std::min(totalTaatsu, remainingMeldsNeeded);

    // 只要有对子，雀头就有了
    int pairBonus = (pairs > 0) ? 1 : 0;

    int shanten = 8 - 2 * melds - effectiveTaatsu - pairBonus;
    return std::max(0, shanten);
}

int MahjongLogic::calculateShanten(const std::vector<Tile>& hand) {
    if (hand.size() > 14 || hand.size() < 13) return -1;
    std::vector<int> counts = tilesToCounts(hand);

    // 14张牌：先判断是否已胡牌
    if (hand.size() == 14 && isWinningHand(counts, false)) return -1;

    // 13张牌：计算向听数
    if (hand.size() == 13) return computeShanten13(counts);

    // 14张牌未胡牌：对每种可能的牌尝试打出一张，取最低向听数
    int best = 8;
    for (int i = 1; i < 30; ++i) {
        if (counts[i] > 0) {
            counts[i]--;
            int s = computeShanten13(counts);
            if (s < best) best = s;
            counts[i]++;
        }
    }
    return best;
}

// 解析 "1筒 2万 3条 4筒" 格式的字符串为 Tile 列表
std::vector<Tile> MahjongLogic::parseTileList(const QString &input) {
    std::vector<Tile> result;
    QStringList parts = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        QString s = part.trimmed();
        if (s.isEmpty()) continue;

        // 提取数字部分和花色部分
        TileSuit suit;
        int value = 0;

        if (s.endsWith("筒")) {
            suit = TileSuit::Dot;
            s.chop(1);
        } else if (s.endsWith("条")) {
            suit = TileSuit::Bamboo;
            s.chop(1);
        } else if (s.endsWith("万")) {
            suit = TileSuit::Character;
            s.chop(1);
        } else {
            continue; // 无法识别的格式，跳过
        }

        bool ok = false;
        value = s.toInt(&ok);
        if (ok && value >= 1 && value <= 9) {
            result.push_back({suit, value});
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}
