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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_DICE_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_DICE_H_

#include <string>

namespace open_spiel {
namespace trictrac {

// Represents the two dice used in Trictrac.
struct Dice {
public:
  Dice(int v1, int v2) : val1(v1), val2(v2) {}

  // Returns true if the two dice have the same value.
  bool IsDouble() const { return val1 == val2; }

  int val1;
  int val2;

  std::string ToString() const;
};

} // namespace trictrac
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_TRICTRAC_TRICTRAC_DICE_H_
