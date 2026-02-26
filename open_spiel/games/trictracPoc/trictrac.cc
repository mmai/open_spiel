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

#include "open_spiel/games/trictrac/trictrac.h"
#include "open_spiel/games/trictrac/trictrac_player.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "open_spiel/abseil-cpp/absl/strings/str_cat.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/games/trictrac/trictrac_rules.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace trictrac {

namespace {
// Facts about the game.

const std::vector<Dice> kChanceOutcomeValues = {
    Dice(1, 2), Dice(2, 1), Dice(1, 3), Dice(3, 1), Dice(1, 4),
    Dice(4, 1), Dice(1, 5), Dice(5, 1), Dice(1, 6), Dice(6, 1),
    Dice(2, 3), Dice(3, 2), Dice(2, 4), Dice(4, 2), Dice(2, 5),
    Dice(5, 2), Dice(2, 6), Dice(6, 2), Dice(3, 4), Dice(4, 3),
    Dice(3, 5), Dice(5, 3), Dice(3, 6), Dice(6, 3), Dice(4, 5),
    Dice(5, 4), Dice(4, 6), Dice(6, 4), Dice(5, 6), Dice(6, 5),
    Dice(1, 1), Dice(2, 2), Dice(3, 3), Dice(4, 4), Dice(5, 5),
    Dice(6, 6)

};

const GameType kGameType{
    /*short_name=*/"trictrac",
    /*long_name=*/"Trictrac",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kExplicitStochastic,
    GameType::Information::kPerfectInformation,
    GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    /*max_num_players=*/kNumPlayers,
    /*min_num_players=*/kNumPlayers,
    /*provides_information_state_string=*/true,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/{} // No parameters
};

std::shared_ptr<const Game> Factory(const GameParameters &params) {
  return std::make_shared<TrictracGame>(params);
}

REGISTER_SPIEL_GAME(kGameType, Factory);

} // namespace

// --- TrictracState ---

TrictracState::TrictracState(std::shared_ptr<const Game> game) : State(game) {}

Player TrictracState::CurrentPlayer() const {
  if (IsTerminal()) {
    return kTerminalPlayerId;
  }
  if (needs_roll_) {
    return kChancePlayerId;
  }
  return static_cast<Player>(current_player_);
}

std::vector<std::pair<Action, double>> TrictracState::ChanceOutcomes() const {
  SPIEL_CHECK_TRUE(IsChanceNode());
  std::vector<std::pair<Action, double>> outcomes;
  outcomes.reserve(kNumChanceOutcomes);
  const double uniform_prob = 1.0 / kNumChanceOutcomes;
  for (Action action = 0; action < kNumChanceOutcomes; ++action) {
    outcomes.push_back({action, uniform_prob});
  }
  return outcomes;
}

std::string TrictracState::ToString() const {
  std::stringstream ss;
  const TrictracPlayer &current_p =
      (current_player_ == kPlayer0) ? player0_ : player1_;
  const TrictracPlayer &opponent_p =
      (current_player_ == kPlayer0) ? player1_ : player0_;

  ss << "Current Player: " << current_p.name << " (holes: " << current_p.holes
     << ", points: " << current_p.points << ")\n";
  ss << "Opponent: " << opponent_p.name << " (holes: " << opponent_p.holes
     << ", points: " << opponent_p.points << ")\n";

  if (needs_roll_) {
    ss << "Dice: Needs roll\n";
  } else {
    ss << "Dice: " << current_dice_.ToString() << "\n";
  }

  ss << board_.ToString();
  return ss.str();
}

bool TrictracState::IsTerminal() const {
  return player0_.holes >= kNumHolesToWin || player1_.holes >= kNumHolesToWin;
}

std::vector<double> TrictracState::Returns() const {
  if (player0_.holes >= kNumHolesToWin) {
    return {1.0, -1.0};
  }
  if (player1_.holes >= kNumHolesToWin) {
    return {-1.0, 1.0};
  }
  return {0.0, 0.0};
}

std::unique_ptr<State> TrictracState::Clone() const {
  return std::make_unique<TrictracState>(*this);
}

void TrictracState::DoApplyAction(Action action_id) {
  if (needs_roll_) {
    // Chance node: action is the outcome of the dice roll.
    current_dice_ = kChanceOutcomeValues[action_id];
    needs_roll_ = false;
    return;
  }

  // Player node: apply the chosen move.
  std::pair<CheckerMove, CheckerMove> moves = ActionToMoveSequence(action_id);
  Color color = (current_player_ == kPlayer0) ? Color::kWhite : Color::kBlack;
  board_.MoveChecker(color, moves.first);
  board_.MoveChecker(color, moves.second);

  // TODO: Update points based on the move and dice roll.

  // Switch player and set up for the next roll.
  current_player_ = OtherPlayer(current_player_);
  needs_roll_ = true;
}

std::vector<Action> TrictracState::LegalActions() const {
  if (needs_roll_) {
    // Chance node: return all possible dice rolls.
    std::vector<Action> actions;
    for (int d1 = 1; d1 <= 6; ++d1) {
      for (int d2 = 1; d2 <= 6; ++d2) {
        // Encode dice roll as an action. A simple way is (d1-1)*6 + (d2-1).
        actions.push_back((d1 - 1) * 6 + (d2 - 1));
      }
    }
    return actions;
  }

  // Player node: return all legal moves.
  std::vector<Action> actions;
  std::vector<std::pair<CheckerMove, CheckerMove>> legal_moves =
      GenerateLegalMoveSequences();
  for (const auto &moves : legal_moves) {
    actions.push_back(MoveSequenceToAction(moves));
  }
  return actions;
}

std::vector<std::pair<CheckerMove, CheckerMove>>
TrictracState::GenerateLegalMoveSequences() const {
  Board board_for_rules = board_;
  Color color_for_rules = Color::kWhite;
  if (current_player_ == kPlayer1) {
    board_for_rules = board_.Mirror();
    color_for_rules = Color::kBlack;
  }

  TrictracRules rules(board_for_rules, current_dice_);
  auto sequences = rules.GetLegalMoveSequences();

  // Mirror back the moves if the player was Black
  if (current_player_ == kPlayer1) {
    for (auto &seq : sequences) {
      seq.first = seq.first.Mirror();
      seq.second = seq.second.Mirror();
    }
  }
  return sequences;
}

Action TrictracState::MoveSequenceToAction(
    const std::pair<CheckerMove, CheckerMove> &moves) const {
  // Encode a move sequence into a single integer.
  // from/to are in [0, 24]. So we use base 25.
  return moves.first.from * 25 * 25 * 25 + moves.first.to * 25 * 25 +
         moves.second.from * 25 + moves.second.to;
}

std::pair<CheckerMove, CheckerMove>
TrictracState::ActionToMoveSequence(Action action_id) const {
  int to2 = action_id % 25;
  action_id /= 25;
  int from2 = action_id % 25;
  action_id /= 25;
  int to1 = action_id % 25;
  action_id /= 25;
  int from1 = action_id;
  return {{from1, to1}, {from2, to2}};
}

// --- Information/Observation ---
std::string TrictracState::ActionToString(Player player,
                                          Action action_id) const {
  // TODO: Implement a proper ActionToString
  return absl::StrCat("Action ", action_id);
}

std::string TrictracState::InformationStateString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, kNumPlayers);
  return ToString();
}

std::string TrictracState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, kNumPlayers);
  return ToString();
}

void TrictracState::ObservationTensor(Player player,
                                      absl::Span<float> values) const {
  // Simplified observation tensor.
  // A full implementation would encode the board, player stats, etc.
  std::fill(values.begin(), values.end(), 0.0f);
}

// --- TrictracGame ---

TrictracGame::TrictracGame(const GameParameters &params)
    : Game(kGameType, params) {}

int TrictracGame::NumDistinctActions() const {
  // Maximum possible action encoding: 24*25*25*25 + 24*25*25 + 24*25 + 24
  return 25 * 25 * 25 * 25;
}

std::vector<int> TrictracGame::ObservationTensorShape() const {
  // Simplified observation tensor shape.
  return {10};
}

} // namespace trictrac
} // namespace open_spiel
