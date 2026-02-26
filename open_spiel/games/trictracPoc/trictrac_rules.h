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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_RULES_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_RULES_H_

#include <utility>
#include <vector>

#include "open_spiel/games/trictrac/trictrac_board.h"
#include "open_spiel/games/trictrac/trictrac_dice.h"
#include "open_spiel/games/trictrac/trictrac_player.h"

namespace open_spiel {
namespace trictrac {

// Enum representing potential rule violations for a move.
enum class MoveError {
  kOpponentCorner,
  kCornerNeedsTwoCheckers,
  kCornerByEffectPossible,
  kExitNeedsAllCheckersOnLastQuarter,
  kExitByEffectPossible,
  kExitNotFarthest,
  kOpponentCanFillQuarter,
  kMustFillQuarter,
  kMustPlayAllDice,
  kMustPlayStrongerDie,
  kOk,
};

// Encapsulates the move generation and validation logic of Trictrac.
// This class always analyzes the board from the perspective of the White player.
// The board must be mirrored for the Black player before being passed in.
class TrictracRules {
 public:
  TrictracRules(const Board& board, Dice dice);

  // Returns all legal move sequences for the given board state and dice.
  std::vector<std::pair<CheckerMove, CheckerMove>> GetLegalMoveSequences();

  // Checks if a given move sequence is allowed according to the rules.
  MoveError IsMoveAllowed(const std::pair<CheckerMove, CheckerMove>& moves);

 private:
  // Generates possible single checker moves for a given die value.
  std::vector<CheckerMove> GetPossibleSingleMoves(int die_val,
                                                  bool with_excedents,
                                                  bool forbid_exits) const;

  // Rule validation helpers.
  MoveError CheckCornerRules(
      const std::pair<CheckerMove, CheckerMove>& moves) const;
  MoveError CheckExitRules(
      const std::pair<CheckerMove, CheckerMove>& moves) const;
  MoveError CheckOpponentCanFillQuarterRule(
      const std::pair<CheckerMove, CheckerMove>& moves) const;
  MoveError CheckMustFillQuarterRule(
      const std::pair<CheckerMove, CheckerMove>& moves) const;

  bool HasCheckersOutsideLastQuarter() const;
  bool CanTakeCornerByEffect() const;
  bool IsMoveByPuissance(
      const std::pair<CheckerMove, CheckerMove>& moves) const;

  Board board_;
  Dice dice_;
};

}  // namespace trictrac
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_RULES_H_
