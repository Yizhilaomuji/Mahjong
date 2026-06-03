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

// 递归计算基础向听数：给定counts数组(已去掉若干面子)，计算还需多少张牌听牌
static int calcBaseShanten(const std::vector<int>& counts, int meldsExtracted) {
    int pairCount = 0;       // 对子数
    int partialSeqCount = 0; // 搭子数(相邻两张/间隔一张)

    std::vector<bool> usedInPartial(30, false);

    // 统计相邻搭子(如 3-4, 5-6) 和 间隔搭子(如 3-5, 4-6)
    for (int s = 0; s < 3; ++s) {
        for (int v = 1; v <= 9; ++v) {
            int i = s * 10 + v;
            // 相邻两张
            if (v <= 8 && counts[i] > 0 && counts[i + 1] > 0) {
                partialSeqCount++;
            }
            // 间隔一张 (坎张搭子)
            if (v <= 7 && counts[i] > 0 && counts[i + 2] > 0) {
                partialSeqCount++;
            }
        }
    }

    // 统计对子
    for (int i = 0; i < 30; ++i) {
        if (counts[i] >= 2) pairCount++;
    }

    int remainingMeldsNeeded = 4 - meldsExtracted;
    int totalTaatsu = partialSeqCount + pairCount;

    // 尝试保留一个对子作为雀头，其余搭子填补面子需求
    int bestShanten = INT_MAX;
    for (int headUsed = 0; headUsed <= (pairCount > 0 ? 1 : 0); ++headUsed) {
        int availableTaatsu = totalTaatsu - headUsed;
        int taatsuFill = std::min(availableTaatsu, remainingMeldsNeeded);
        int unfilledMelds = remainingMeldsNeeded - taatsuFill;
        int needPair = (headUsed == 1) ? 0 : 1;
        int shanten = unfilledMelds * 2 + needPair;
        if (shanten < bestShanten) bestShanten = shanten;
    }

    return bestShanten;
}

// 递归尝试取走面子，求最小向听数
static int shantenRecursive(std::vector<int>& counts, int melds) {
    int best = calcBaseShanten(counts, melds);
    if (melds >= 4) return best;

    // 找第一个有牌的位置
    int first = 0;
    while (first < 30 && counts[first] == 0) first++;
    if (first >= 30) return best;

    // 尝试刻子
    if (counts[first] >= 3) {
        counts[first] -= 3;
        best = std::min(best, shantenRecursive(counts, melds + 1));
        counts[first] += 3;
    }

    // 尝试顺子
    if (first % 10 <= 7 && first < 28 &&
        counts[first + 1] > 0 && counts[first + 2] > 0) {
        counts[first]--;
        counts[first + 1]--;
        counts[first + 2]--;
        best = std::min(best, shantenRecursive(counts, melds + 1));
        counts[first]++;
        counts[first + 1]++;
        counts[first + 2]++;
    }

    return best;
}

int MahjongLogic::calculateShanten(const std::vector<Tile>& hand) {
    if (hand.size() > 14 || hand.size() < 13) return -1;
    std::vector<int> counts = tilesToCounts(hand);

    // 若14张牌已胡牌，向听数为 -1
    if (hand.size() == 14 && isWinningHand(counts, false)) return -1;

    return shantenRecursive(counts, 0);
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
