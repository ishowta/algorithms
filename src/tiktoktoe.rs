use fixedbitset::FixedBitSet;
use rand::prelude::*;
use std::collections::HashMap;

struct PutEvalState {
    index: usize,
    count: usize,
    sum_award: isize,
}

struct BoardState {
    put_states: Vec<PutEvalState>,
}

#[derive(PartialEq, Eq)]
enum Player {
    TIK,
    TOK,
}

impl Player {
    fn toggle(&mut self) {
        *self = match self {
            Player::TIK => Player::TOK,
            Player::TOK => Player::TIK,
        };
    }
}

impl std::fmt::Debug for Player {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::result::Result<(), std::fmt::Error> {
        write!(
            f,
            "{}",
            match self {
                Player::TIK => "⚪",
                Player::TOK => "❌",
            }
        )
    }
}

impl std::fmt::Display for Player {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::result::Result<(), std::fmt::Error> {
        write!(
            f,
            "{}",
            match self {
                Player::TIK => "⚪",
                Player::TOK => "❌",
            }
        )
    }
}

type BoardBit = FixedBitSet;

#[derive(PartialEq, Eq, Hash, Clone)]
struct Board {
    tik: BoardBit,
    tok: BoardBit,
}

impl Board {
    fn new() -> Board {
        Board {
            tik: BoardBit::with_capacity(9),
            tok: BoardBit::with_capacity(9),
        }
    }
    fn check(&self) -> Option<Player> {
        let check_ = |b: &BoardBit| -> bool {
            let win_pattern_list: [FixedBitSet; 8] = [
                BoardBit::with_capacity_and_blocks(9, vec![0b111000000]),
                BoardBit::with_capacity_and_blocks(9, vec![0b000111000]),
                BoardBit::with_capacity_and_blocks(9, vec![0b000000111]),
                BoardBit::with_capacity_and_blocks(9, vec![0b100010001]),
                BoardBit::with_capacity_and_blocks(9, vec![0b001010100]),
                BoardBit::with_capacity_and_blocks(9, vec![0b100100100]),
                BoardBit::with_capacity_and_blocks(9, vec![0b010010010]),
                BoardBit::with_capacity_and_blocks(9, vec![0b001001001]),
            ];
            win_pattern_list.iter().any(|x| x & b == *x)
        };
        if check_(&self.tik) {
            Some(Player::TIK)
        } else if check_(&self.tok) {
            Some(Player::TOK)
        } else {
            None
        }
    }
}

impl std::fmt::Debug for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::result::Result<(), std::fmt::Error> {
        for i in 0..9 {
            if self.tik[i] {
                write!(f, "{}", Player::TIK)?;
            } else if self.tok[i] {
                write!(f, "{}", Player::TOK)?;
            } else {
                write!(f, "　")?;
            }
            if (i + 1) % 3 == 0 {
                writeln!(f)?;
            }
        }
        writeln!(f)?;
        Ok(())
    }
}

impl std::ops::Index<&Player> for Board {
    type Output = BoardBit;
    fn index(&self, player: &Player) -> &Self::Output {
        match player {
            Player::TIK => &self.tik,
            Player::TOK => &self.tok,
        }
    }
}

impl std::ops::IndexMut<&Player> for Board {
    fn index_mut(&mut self, player: &Player) -> &mut Self::Output {
        match player {
            Player::TIK => &mut self.tik,
            Player::TOK => &mut self.tok,
        }
    }
}

pub fn run() {
    let mut rng = rand::thread_rng();
    let mut board = Board::new();
    let mut player = Player::TIK;
    let mut board_states = HashMap::<Board, BoardState>::new();

    loop {
        let board_state = board_states.entry(board.clone()).or_insert(BoardState {
            put_states: (0..9).fold(vec![], |mut states, i| {
                if !board.tik[i] && !board.tok[i] {
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

        let random_pos = put_states[rng.gen_range(0, put_states.len())].index;

        board[&player].toggle(random_pos);

        if board.check().is_some() {
            break;
        }

        player.toggle();
    }

    println!("{:?}", board);
    let result = board.check();
    println!("Win: {:?}", result)
}
