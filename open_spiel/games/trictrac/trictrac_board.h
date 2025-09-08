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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_BOARD_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_BOARD_H_

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "open_spiel/games/trictrac/trictrac_player.h"

namespace open_spiel {
namespace trictrac {

// Represents a move of a single checker.
// Field positions are 1-24. 0 represents bearing off or the bar.
struct CheckerMove {
  CheckerMove(int from, int to);
  int from = 0;
  int to = 0;

  // Get the mirrored move (from the opponent's perspective).
  CheckerMove Mirror() const;

  // Returns true if the move is a bear-off move.
  bool IsExit() const;
};

const CheckerMove kEmptyMove(0, 0);

// Represents the Trictrac game board.
class Board {
 public:
  Board();

  // Get the mirrored board (from the opponent's perspective).
  Board Mirror() const;

  // Moves a checker for a given color.
  void MoveChecker(Color color, CheckerMove move);

  // Adds a checker to a field.
  void AddChecker(Color color, int field);

  // Removes a checker from a field.
  void RemoveChecker(Color color, int field);

  // Checks if a field is blocked for a player.
  bool IsBlocked(Color color, int field) const;

  // Returns a list of fields containing checkers of the given color.
  // The pair contains the field number (1-24) and the count of checkers.
  std::vector<std::pair<int, int>> GetPlayerCheckers(Color color) const;

  // Returns true if any quarter of the board is filled by the player.
  bool AnyQuarterFilled(Color color) const;

  // Returns true if the quarter containing the given field is filled.
  bool IsQuarterFilled(Color color, int field) const;

  // Checks if a quarter is still possible to be filled.
  bool IsQuarterFillable(Color color, int field) const;

  // For testing: sets the board positions directly.
  void SetPositions(const std::array<int, 24>& positions);

 private:
  // Returns the 6 fields of the quarter containing the given field.
  std::array<int, 6> GetQuarterFields(int field) const;

  // Positive values for White, negative for Black.
  std::array<int, 24> positions_;
};

}  // namespace trictrac
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_BOARD_H_
