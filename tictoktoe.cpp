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
};

// tik win 1, tok win -1, 終わってない 0
int check(Board board)
{
    // TODO: 実装
    return 0;
}

int main()
{
    std::random_device rnd;
    std::mt19937 mt(rnd());

    Board board;
    int player = 1; // tik 1, tok 2
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
        if (check(board) != 0)
            break;

        // プレイヤー交代
        player = player == 1 ? 2 : 1;
    }
    for (size_t i = 0; i < 9; i++)
    {
        std::cout << (board.tik[i] ? "⚪" : board.tok[i] ? "❌" : "　");
        if ((i + 1) % 3 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}
