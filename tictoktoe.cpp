/**
 * https://qiita.com/kaityo256/items/50155365323aaaaa3b3a
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <random>
#include <bitset>
#include <boost/unordered_map.hpp>
#include <optional>

using std::cout, std::endl;
using std::optional;

struct PutEvalState
{
    int index;
    int count;
    int sumAward;
};

struct BoardEvalState
{
    std::vector<PutEvalState> putStats;
};

enum Player
{
    TIK = 1,
    TOK = -1
};

struct Board
{
    std::bitset<9> tik;
    std::bitset<9> tok;

    Board() : tik(0), tok(0) {}

    bool operator==(const Board &rhs) const
    {
        return tik == rhs.tik && tok == rhs.tok;
    }

    friend std::size_t hash_value(Board const &rhs)
    {
        std::size_t seed = 0;
        std::hash<std::bitset<9>> hash_fn;
        boost::hash_combine(seed, hash_fn(rhs.tik));
        boost::hash_combine(seed, hash_fn(rhs.tok));

        return seed;
    }

    // tik win 1, tok win -1, 終わってない 0
    std::optional<Player> check()
    {
        auto check_ = [](std::bitset<9> b) -> bool {
            auto winList = {std::bitset<9>(0b111000000),
                            std::bitset<9>(0b000111000),
                            std::bitset<9>(0b000000111),
                            std::bitset<9>(0b100010001),
                            std::bitset<9>(0b001010100),
                            std::bitset<9>(0b100100100),
                            std::bitset<9>(0b010010010),
                            std::bitset<9>(0b001001001)};
            return std::any_of(begin(winList), end(winList), [&](std::bitset<9> x) { return (x & b) == x; });
        };
        return check_(tik) ? TIK : check_(tok) ? TOK : optional<Player>();
    }
};

int main()
{
    std::random_device rnd;
    std::mt19937 mt(rnd());

    Board board;
    Player player = TIK;
    boost::unordered_map<Board, BoardEvalState> boardStats;

    while (true)
    {
        // BoardStatsを探して、無かったら新しく作る
        auto itr = boardStats.find(board);
        if (itr == boardStats.end())
        {
            BoardEvalState newBoardStats;
            newBoardStats.putStats = {};
            for (int i = 0; i < 9; i++)
            {
                if (board.tik[i] == 0 && board.tok[i] == 0)
                    newBoardStats.putStats.push_back(PutEvalState{i, 0, 0});
            }

            auto res = boardStats.insert(std::make_pair(board, newBoardStats));
            itr = res.first;
        }

        auto &boardEvalState = (*itr).second;

        auto &putStats = boardEvalState.putStats;

        // 置くとこが無くなったら終わり
        if (putStats.size() == 0)
            break;

        // ランダムに置く
        std::uniform_int_distribution<> rand(0, putStats.size() - 1);
        auto &posState = putStats.at(rand(mt));
        int pos = posState.index;
        player == 1 ? board.tik[pos].flip() : board.tok[pos].flip();

        // 勝敗が決まったら終わり
        if (board.check())
            break;

        // プレイヤー交代
        player = Player(-player);
    }

    auto res = board.check();
    for (size_t i = 0; i < 9; i++)
    {
        cout << (board.tik[i] ? "⚪" : board.tok[i] ? "❌" : "　");
        if ((i + 1) % 3 == 0)
            cout << endl;
    }
    cout << endl;
    cout << "Win: " << (res ? res.value() == TIK ? "⚪" : "❌" : "") << endl;
}
