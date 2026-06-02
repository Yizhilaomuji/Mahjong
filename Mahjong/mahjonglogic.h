#ifndef MAHJONGLOGIC_H
#define MAHJONGLOGIC_H

#include <vector>
#include <map>
#include <string>
#include <QString>

// 四川麻将只有筒(Dot)、条(Bamboo)、万(Character)
enum class TileSuit {
    Dot,
    Bamboo,
    Character
};

struct Tile {
    TileSuit suit;
    int value; // 1-9

    bool operator==(const Tile& other) const {
        return suit == other.suit && value == other.value;
    }
    bool operator<(const Tile& other) const {
        if (suit != other.suit) return suit < other.suit;
        return value < other.value;
    }
    
    QString toString() const {
        QString s = QString::number(value);
        if (suit == TileSuit::Dot) s += "筒";
        else if (suit == TileSuit::Bamboo) s += "条";
        else s += "万";
        return s;
    }
};

class MahjongLogic {
public:
    // 生成一套有意义的训练手牌 (保证必定存在可以听牌的打法)
    static std::vector<Tile> generateTrainingHand();
    
    // 给定14张牌，判断打出哪张牌能听什么牌
    // 返回值: 键为建议打出的牌，值为打出该牌后能听的牌列表
    static std::map<Tile, std::vector<Tile>> getDiscardSuggestions(const std::vector<Tile>& hand);

private:
    // 将牌组转换为整数统计数组，大小30 (0-9筒, 10-19条, 20-29万)
    static std::vector<int> tilesToCounts(const std::vector<Tile>& tiles);
    // 递归判断当前牌是否胡牌
    static bool isWinningHand(std::vector<int> counts, bool hasPair);
    // 给定13张牌，返回所有能胡的牌
    static std::vector<Tile> getWaits(const std::vector<Tile>& hand13);
};

#endif // MAHJONGLOGIC_H
