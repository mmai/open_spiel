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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_POINTS_RULES_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_POINTS_RULES_H_

#include <map>
#include <utility>
#include <vector>

#include "open_spiel/games/trictrac/trictrac_board.h"
#include "open_spiel/games/trictrac/trictrac_dice.h"
#include "open_spiel/games/trictrac/trictrac_player.h"
#include "open_spiel/games/trictrac/trictrac_rules.h"

namespace open_spiel {
namespace trictrac {

// Enum representing all possible scoring events ("Jans").
enum class Jan {
  kFilledQuarter,
  kTrueHitSmallJan,
  kTrueHitBigJan,
  kTrueHitOpponentCorner,
  kFirstPlayerToExit,
  kSixTables,
  kTwoTables,
  kMezeas,
  kFalseHitSmallJan,
  kFalseHitBigJan,
  kContreTwoTables,
  kContreMezeas,
  kHelplessMan,
};

// Returns the point value for a given Jan.
int GetJanPoints(Jan jan, bool is_double);

// A map of possible Jans to the move sequences that can achieve them.
using PossibleJans =
    std::map<Jan, std::vector<std::pair<CheckerMove, CheckerMove>>>;

// Encapsulates the point scoring logic of Trictrac.
// This class always analyzes the board from the perspective of the White player.
class TrictracPointsRules {
 public:
  TrictracPointsRules(const Board& board, Dice dice, int dice_rolls_count);

  // Returns all possible Jans for the current state.
  PossibleJans GetPossibleJans();

  // Calculates the points for the current player and the opponent.
  std::pair<int, int> GetPoints();

 private:
  void FindHitJans(PossibleJans* jans);
  void FindFilledQuarterJans(PossibleJans* jans);
  void FindRareJans(PossibleJans* jans);
  void FindHelplessManJan(PossibleJans* jans);

  Board board_;
  Dice dice_;
  int dice_rolls_count_;
  TrictracRules move_rules_;
};

}  // namespace trictrac
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_POINTS_RULES_H_
