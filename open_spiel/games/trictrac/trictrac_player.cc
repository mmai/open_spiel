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

#include "open_spiel/games/trictrac/trictrac_player.h"

namespace open_spiel {
namespace trictrac {

Color OpponentColor(Color color) {
  return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

Player OtherPlayer(Player player) {
  switch (player) {
    case Player::kPlayer0:
      return Player::kPlayer1;
    case Player::kPlayer1:
      return Player::kPlayer0;
    default:
      return Player::kNobody;
  }
}

TrictracPlayer::TrictracPlayer(std::string name, Color color)
    : name(name), color(color) {}

}  // namespace trictrac
}  // namespace open_spiel
