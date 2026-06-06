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
    // 计算胡牌番数 (基础)
    static int calculateFanTypes(const std::vector<Tile>& hand14);

    // 生成一套有意义的训练手牌 (保证必定存在可以听牌的打法)
    static std::vector<Tile> generateTrainingHand();

    // 给定14张牌，判断打出哪张牌能听什么牌
    // 返回值: 键为建议打出的牌，值为打出该牌后能听的牌列表
    static std::map<Tile, std::vector<Tile>> getDiscardSuggestions(const std::vector<Tile>& hand);

    // 14张牌时，返回听牌数量最多的最佳出牌方案
    static std::vector<std::pair<Tile, std::vector<Tile>>> getBestDiscardOptions(const std::vector<Tile>& hand);

    // 生成108张牌并洗牌
    static std::vector<Tile> generateWall();

    // 判断14张牌是否胡牌
    static bool canWin(const std::vector<Tile>& hand14);

    // 统计手牌中某花色的张数
    static int countSuit(const std::vector<Tile>& hand, TileSuit suit);

    // 判断某张牌在手牌中是否孤立 (同花色无 value±2 以内的其他牌)
    static bool isIsolated(const std::vector<Tile>& hand, Tile tile);

    // 判断手牌中是否包含某张牌 (value + suit 匹配)
    static bool hasTile(const std::vector<Tile>& hand, Tile tile);

    // 从手牌中移除一张匹配的牌，返回是否成功
    static bool removeTile(std::vector<Tile>& hand, Tile tile);

    // 将牌组转换为整数统计数组，大小30 (0-9筒, 10-19条, 20-29万)
    static std::vector<int> tilesToCounts(const std::vector<Tile>& tiles);

    // 计算向听数 (0=听牌, 1=一向听, 2=二向听 ...)
    static int calculateShanten(const std::vector<Tile>& hand);

    // 给定13张牌，返回所有能胡的牌
    static std::vector<Tile> getWaits(const std::vector<Tile>& hand13);

    // 计算某种牌在当前手牌中剩余的张数
    static int remainingCopies(const std::vector<Tile>& hand, Tile tile);

    // 计算给定听牌类型列表中，当前手牌剩余的听牌张数总和
    static int countRemainingWaitTiles(const std::vector<Tile>& hand, const std::vector<Tile>& waits);

    // 计算打出指定牌后，听牌张数总和
    static int countRemainingWaitTilesAfterDiscard(const std::vector<Tile>& hand, Tile discard);

    // 文本解析: "1筒 2万 3条" → Tile列表
    static std::vector<Tile> parseTileList(const QString &input);

private:
    // 递归判断当前牌是否胡牌
    static bool isWinningHand(std::vector<int> counts, bool hasPair);
};

#endif // MAHJONGLOGIC_H
