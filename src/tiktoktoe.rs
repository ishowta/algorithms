use bitvec::prelude::*;
use rand::prelude::*;
use std::collections::HashMap;

struct PutEvalState {
    index: u8,
    count: u32,
    sum_award: i32,
}

struct BoardEvalState {
    put_states: Vec<PutEvalState>,
}

#[derive(PartialEq, Eq)]
enum Player {
    TIK,
    TOK,
}

type BoardBit = BitVec;

#[derive(PartialEq, Eq, Hash, Clone)]
struct Board {
    tik: BoardBit,
    tok: BoardBit,
}

impl Board {
    fn new() -> Board {
        Board {
            tik: bitvec![0, 0, 0, 0, 0, 0, 0, 0, 0],
            tok: bitvec![0, 0, 0, 0, 0, 0, 0, 0, 0],
        }
    }
    fn check(&self) -> Option<Player> {
        let check_ = |b: &BoardBit| {
            let win_pattern_list = [
                bitvec![1, 1, 1, 0, 0, 0, 0, 0, 0],
                bitvec![0, 0, 0, 1, 1, 1, 0, 0, 0],
                bitvec![0, 0, 0, 0, 0, 0, 1, 1, 1],
                bitvec![1, 0, 0, 0, 1, 0, 0, 0, 1],
                bitvec![0, 0, 1, 0, 1, 0, 1, 0, 0],
                bitvec![1, 0, 0, 1, 0, 0, 1, 0, 0],
                bitvec![0, 1, 0, 0, 1, 0, 0, 1, 0],
                bitvec![0, 0, 1, 0, 0, 1, 0, 0, 1],
            ];
            win_pattern_list
                .iter()
                .any(|x| &(x.clone() & b.clone()) == x)
        };
        if check_(&self.tik) {
            Some(Player::TIK)
        } else if check_(&self.tok) {
            Some(Player::TOK)
        } else {
            None
        }
    }

    fn print(&self) {
        for i in 0..9 {
            print!(
                "{}",
                if self.tik[i] {
                    "⚪"
                } else if self.tok[i] {
                    "❌"
                } else {
                    "　"
                }
            );
            if (i + 1) % 3 == 0 {
                println!()
            }
        }
        println!();
    }
}

pub fn run() {
    let mut rng = rand::thread_rng();
    let mut board = Board::new();
    let mut player = Player::TIK;
    let mut board_states = HashMap::<Board, BoardEvalState>::new();

    loop {
        let board_state = board_states.entry(board.clone()).or_insert(BoardEvalState {
            put_states: (0..9).fold(vec![], |mut states, i| {
                if !board.tik[usize::from(i)] && !board.tok[usize::from(i)] {
                    states.push(PutEvalState {
                        index: i,
                        count: 0,
                        sum_award: 0,
                    })
                };
                states
            }),
        });
        let put_states = &board_state.put_states;

        if put_states.is_empty() {
            break;
        }

        let random_pos: usize = put_states[rng.gen_range(0, put_states.len())].index.into();

        match player {
            Player::TIK => {
                board.tik.insert(random_pos, !board.tik[random_pos]);
                board.tik.remove(random_pos + 1);
            }
            Player::TOK => {
                board.tok.insert(random_pos, !board.tok[random_pos]);
                board.tok.remove(random_pos + 1);
            }
        }

        if board.check().is_some() {
            break;
        }

        player = match player {
            Player::TIK => Player::TOK,
            Player::TOK => Player::TIK,
        };
    }

    board.print();
    let result = board.check();
    println!(
        "Win: {}",
        match result {
            Some(Player::TIK) => "⚪",
            Some(Player::TOK) => "❌",
            None => "",
        }
    )
}
