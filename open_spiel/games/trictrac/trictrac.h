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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_H_

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "open_spiel/spiel.h"
#include "open_spiel/games/trictrac/trictrac_board.h"
#include "open_spiel/games/trictrac/trictrac_dice.h"
#include "open_spiel/games/trictrac/trictrac_player.h"

namespace open_spiel {
namespace trictrac {

// Constants
constexpr int kNumPlayers = 2;
constexpr int kNumHolesToWin = 12;

class TrictracGame;

// State of a Trictrac game.
class TrictracState : public State {
 public:
  TrictracState(std::shared_ptr<const Game> game);
  TrictracState(const TrictracState&) = default;

  Player CurrentPlayer() const override;
  std::string ActionToString(Player player, Action action_id) const override;
  std::string ToString() const override;
  bool IsTerminal() const override;
  std::vector<double> Returns() const override;
  std::string InformationStateString(Player player) const override;
  std::string ObservationString(Player player) const override;
  void ObservationTensor(Player player,
                         absl::Span<float> values) const override;
  std::unique_ptr<State> Clone() const override;

 protected:
  void DoApplyAction(Action action_id) override;
  std::vector<Action> LegalActions() const override;

 private:
  // Helper functions for move generation and action encoding.
  std::vector<std::pair<CheckerMove, CheckerMove>> GenerateLegalMoveSequences() const;
  Action MoveSequenceToAction(const std::pair<CheckerMove, CheckerMove>& moves) const;
  std::pair<CheckerMove, CheckerMove> ActionToMoveSequence(Action action_id) const;


  Board board_;
  TrictracPlayer player0_{"Player 0", Color::kWhite};
  TrictracPlayer player1_{"Player 1", Color::kBlack};
  Player current_player_ = Player::kPlayer0;
  Dice current_dice_{1, 1}; // Default dice
  bool needs_roll_ = true;
};

// Game object for Trictrac.
class TrictracGame : public Game {
 public:
  explicit TrictracGame(const GameParameters& params);
  int NumDistinctActions() const override;
  std::unique_ptr<State> NewInitialState() const override {
    return std::make_unique<TrictracState>(shared_from_this());
  }
  int NumPlayers() const override { return kNumPlayers; }
  double MinUtility() const override { return -1.0; }
  double MaxUtility() const override { return 1.0; }
  std::shared_ptr<const Game> Clone() const override {
    return std::make_shared<TrictracGame>(*this);
  }
  std::vector<int> ObservationTensorShape() const override;
  int MaxGameLength() const override { return 500; } // Estimate
};

}  // namespace trictrac
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_TRICTRAC_H_