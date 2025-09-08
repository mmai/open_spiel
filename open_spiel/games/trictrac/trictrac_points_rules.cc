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

#include "open_spiel/games/trictrac/trictrac_points_rules.h"

namespace open_spiel {
namespace trictrac {

int GetJanPoints(Jan jan, bool is_double) {
  switch (jan) {
    case Jan::kFalseHitSmallJan:
    case Jan::kContreTwoTables:
    case Jan::kContreMezeas:
      return is_double ? -6 : -4;
    case Jan::kFalseHitBigJan:
    case Jan::kHelplessMan:
      return is_double ? -4 : -2;
    case Jan::kTrueHitBigJan:
      return is_double ? 4 : 2;
    default:
      return is_double ? 6 : 4;
  }
}

TrictracPointsRules::TrictracPointsRules(const Board& board, Dice dice,
                                         int dice_rolls_count)
    : board_(board),
      dice_(dice),
      dice_rolls_count_(dice_rolls_count),
      move_rules_(board, dice) {}

PossibleJans TrictracPointsRules::GetPossibleJans() {
  PossibleJans jans;
  FindHitJans(&jans);
  FindFilledQuarterJans(&jans);
  FindRareJans(&jans);
  FindHelplessManJan(&jans);
  return jans;
}

std::pair<int, int> TrictracPointsRules::GetPoints() {
  PossibleJans jans = GetPossibleJans();
  int player_points = 0;
  int opponent_points = 0;

  for (const auto& [jan, moves] : jans) {
    int points = GetJanPoints(jan, dice_.IsDouble()) * moves.size();
    if (points > 0) {
      player_points += points;
    } else {
      opponent_points -= points;
    }
  }
  return {player_points, opponent_points};
}

void TrictracPointsRules::FindHitJans(PossibleJans* jans) {
  // Simplified placeholder implementation for finding "hit" jans.
  // A full implementation would require the complex recursive logic from the
  // Rust code (`get_jans_by_ordered_dice`).
}

void TrictracPointsRules::FindFilledQuarterJans(PossibleJans* jans) {
  // Simplified placeholder implementation.
  // A full implementation would use `move_rules_` to find all move sequences
  // that result in a filled quarter.
}

void TrictracPointsRules::FindRareJans(PossibleJans* jans) {
  // Simplified placeholder implementation for rare jans like Six Tables,
  // Two Tables, Mezeas.
}

void TrictracPointsRules::FindHelplessManJan(PossibleJans* jans) {
  if (move_rules_.GetLegalMoveSequences().empty()) {
    (*jans)[Jan::kHelplessMan].push_back({kEmptyMove, kEmptyMove});
  }
}

}  // namespace trictrac
}  // namespace open_spiel
