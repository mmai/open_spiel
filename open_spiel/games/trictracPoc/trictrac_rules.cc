// Copyright 2024 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "open_spiel/games/trictrac/trictrac_rules.h"

#include <algorithm>
#include <vector>

#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace trictrac {

TrictracRules::TrictracRules(const Board& board, Dice dice)
    : board_(board), dice_(dice) {}

std::vector<std::pair<CheckerMove, CheckerMove>>
TrictracRules::GetLegalMoveSequences() {
  // This is a simplified version of the complex move generation logic.
  // A full implementation would recursively check all combinations and apply
  // all the specific Trictrac rules.
  std::vector<std::pair<CheckerMove, CheckerMove>> legal_sequences;

  int d1 = dice_.val1;
  int d2 = dice_.val2;

  // Try playing d1 then d2
  for (const auto& move1 : GetPossibleSingleMoves(d1, true, false)) {
    Board board_after_move1 = board_;
    board_after_move1.MoveChecker(Color::kWhite, move1);
    for (const auto& move2 : GetPossibleSingleMoves(d2, true, false)) {
      std::pair<CheckerMove, CheckerMove> sequence = {move1, move2};
      if (IsMoveAllowed(sequence) == MoveError::kOk) {
        legal_sequences.push_back(sequence);
      }
    }
  }

  // Try playing d2 then d1
  if (d1 != d2) {
    for (const auto& move1 : GetPossibleSingleMoves(d2, true, false)) {
      Board board_after_move1 = board_;
      board_after_move1.MoveChecker(Color::kWhite, move1);
      for (const auto& move2 : GetPossibleSingleMoves(d1, true, false)) {
        std::pair<CheckerMove, CheckerMove> sequence = {move1, move2};
        if (IsMoveAllowed(sequence) == MoveError::kOk) {
          legal_sequences.push_back(sequence);
        }
      }
    }
  }

  // TODO: Handle "tout d'une" moves (playing both dice for one checker).
  // TODO: Handle cases where only one die can be played.
  // TODO: Handle doublets correctly (playing four times).

  return legal_sequences;
}

MoveError TrictracRules::IsMoveAllowed(
    const std::pair<CheckerMove, CheckerMove>& moves) {
  MoveError error = CheckCornerRules(moves);
  if (error != MoveError::kOk) return error;

  error = CheckExitRules(moves);
  if (error != MoveError::kOk) return error;

  error = CheckOpponentCanFillQuarterRule(moves);
  if (error != MoveError::kOk) return error;

  error = CheckMustFillQuarterRule(moves);
  if (error != MoveError::kOk) return error;

  // TODO: Implement checks for MustPlayAllDice and MustPlayStrongerDie.

  return MoveError::kOk;
}

std::vector<CheckerMove> TrictracRules::GetPossibleSingleMoves(
    int die_val, bool with_excedents, bool forbid_exits) const {
  std::vector<CheckerMove> moves;
  std::vector<std::pair<int, int>> checkers =
      board_.GetPlayerCheckers(Color::kWhite);

  for (const auto& [field, count] : checkers) {
    int dest = field + die_val;
    if (dest > 24) {
      if (with_excedents && !forbid_exits) {
        dest = 0;  // Bear off
      } else {
        continue;
      }
    }

    if (!board_.IsBlocked(Color::kWhite, dest)) {
      moves.emplace_back(field, dest);
    }
  }
  return moves;
}

MoveError TrictracRules::CheckCornerRules(
    const std::pair<CheckerMove, CheckerMove>& moves) const {
  // Simplified placeholder implementation.
  return MoveError::kOk;
}

MoveError TrictracRules::CheckExitRules(
    const std::pair<CheckerMove, CheckerMove>& moves) const {
  // Simplified placeholder implementation.
  if (moves.first.IsExit() || moves.second.IsExit()) {
    if (HasCheckersOutsideLastQuarter()) {
      return MoveError::kExitNeedsAllCheckersOnLastQuarter;
    }
  }
  return MoveError::kOk;
}

MoveError TrictracRules::CheckOpponentCanFillQuarterRule(
    const std::pair<CheckerMove, CheckerMove>& moves) const {
  int farthest = std::max(moves.first.to, moves.second.to);
  if (farthest > 12 && board_.IsQuarterFillable(Color::kBlack, farthest)) {
    return MoveError::kOpponentCanFillQuarter;
  }
  return MoveError::kOk;
}

MoveError TrictracRules::CheckMustFillQuarterRule(
    const std::pair<CheckerMove, CheckerMove>& moves) const {
  // Simplified placeholder implementation.
  return MoveError::kOk;
}

bool TrictracRules::HasCheckersOutsideLastQuarter() const {
  for (const auto& [field, count] : board_.GetPlayerCheckers(Color::kWhite)) {
    if (field < 19) {
      return true;
    }
  }
  return false;
}

bool TrictracRules::CanTakeCornerByEffect() const {
  // Simplified placeholder implementation.
  return false;
}

bool TrictracRules::IsMoveByPuissance(
    const std::pair<CheckerMove, CheckerMove>& moves) const {
  // Simplified placeholder implementation.
  return false;
}

}  // namespace trictrac
}  // namespace open_spiel
