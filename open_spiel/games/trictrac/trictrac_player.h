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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_PLAYER_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_PLAYER_H_

#include "open_spiel/spiel_utils.h"
#include <string>

namespace open_spiel {
namespace trictrac {

// Represents the color of the checkers for a player.
enum class Color {
  kWhite,
  kBlack,
};

// Returns the opponent's color.
Color OpponentColor(Color color);

// Represents a player in the game, mapping to Player 0 and Player 1.
inline constexpr const int kPlayer0 = 1;
inline constexpr const int kPlayer1 = 2;
// Represents neither player, e.g., at the start or end of a game.
inline constexpr const int kNobody = 0;

// Returns the other player.
Player OtherPlayer(Player player);

// Struct for storing player-related data.
class TrictracPlayer {
public:
  TrictracPlayer(std::string name, Color color);

  std::string name;
  Color color;
  int points = 0;
  int holes = 0;
  bool can_bredouille = true;
  bool can_big_bredouille = true;
  // Number of dice rolls since the beginning of the current setting.
  int dice_roll_count = 0;
};

} // namespace trictrac
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_PLAYER_H_
