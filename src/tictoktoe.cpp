#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

using std::cout, std::endl, std::optional, std::make_pair;

struct PutEvalState {
    int index;
    int count;
    int sum_award;
};

struct BoardState {
    std::vector<PutEvalState> put_states;
};

enum Player { TIK, TOK };

typedef std::bitset<9> BoardBit;

struct Board {
    BoardBit tik;
    BoardBit tok;

    Board() : tik(0), tok(0) {}

    bool operator==(const Board &rhs) const {
        return tik == rhs.tik && tok == rhs.tok;
    }

    std::optional<Player> check() const {
        auto check_ = [](BoardBit b) -> bool {
            auto win_pattern_list = {
                BoardBit(0b111000000), BoardBit(0b000111000),
                BoardBit(0b000000111), BoardBit(0b100010001),
                BoardBit(0b001010100), BoardBit(0b100100100),
                BoardBit(0b010010010), BoardBit(0b001001001)};
            return std::any_of(begin(win_pattern_list), end(win_pattern_list),
                               [&](BoardBit x) { return (x & b) == x; });
        };
        return check_(tik) ? TIK : check_(tok) ? TOK : optional<Player>();
    }

    void print() const {
        for (size_t i = 0; i < 9; i++) {
            cout << (this->tik[i] ? "⚪" : this->tok[i] ? "❌" : "　");
            if ((i + 1) % 3 == 0)
                cout << endl;
        }
        cout << endl;
    }
};

struct BoardStateHash {
    typedef std::size_t result_type;

    std::size_t operator()(Board const &rhs) const {
        std::hash<BoardBit> hash_fn;
        return hash_fn(rhs.tik) ^ hash_fn(rhs.tok);
    }
};

int main() {
    std::random_device rnd;
    std::mt19937 mt(rnd());

    Board board;
    Player player = TIK;
    std::unordered_map<Board, BoardState, BoardStateHash> board_states;

    while (true) {
        // board_statesを探して、無かったら新しく作る
        auto &board_state = ([&]() -> BoardState & {
            auto res = board_states.find(board);
            if (res != board_states.end())
                return std::get<1>(*res);

            BoardState newboard_state;

            for (int i = 0; i < 9; i++) {
                if (board.tik[i] == 0 && board.tok[i] == 0)
                    newboard_state.put_states.push_back(PutEvalState{i, 0, 0});
            }

            auto ref = board_states.insert(make_pair(board, newboard_state));
            return std::get<1>(*ref.first);
        })();

        auto &put_states = board_state.put_states;

        // 置くとこが無くなったら終わり
        if (put_states.size() == 0)
            break;

        // ランダムに置く
        std::uniform_int_distribution<> rand(0, put_states.size() - 1);
        int pos = put_states.at(rand(mt)).index;
        player == TIK ? board.tik[pos].flip() : board.tok[pos].flip();

        // 勝敗が決まったら終わり
        if (board.check())
            break;

        // プレイヤー交代
        player = player == TIK ? TOK : TIK;
    }

    auto res = board.check();
    board.print();
    cout << "Win: " << (res ? res.value() == TIK ? "⚪" : "❌" : "") << endl;
}
