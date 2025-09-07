// Copyright 2019 DeepMind Technologies Limited
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

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "open_spiel/abseil-cpp/absl/strings/str_cat.h"
#include "open_spiel/abseil-cpp/absl/strings/str_join.h"
#include "open_spiel/abseil-cpp/absl/types/span.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/observer.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_globals.h"
#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace trictrac {
namespace {

const std::vector<std::vector<int>> kChanceOutcomeValues = {
    {1, 2}, {2, 1}, {1, 3}, {3, 1}, {1, 4}, {4, 1},
    {1, 5}, {5, 1}, {1, 6}, {6, 1}, {2, 3}, {3, 2},
    {2, 4}, {4, 2}, {2, 5}, {5, 2}, {2, 6}, {6, 2},
    {3, 4}, {4, 3}, {3, 5}, {5, 3}, {3, 6}, {6, 3},
    {4, 5}, {5, 4}, {4, 6}, {6, 4}, {5, 6}, {6, 5},
    {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}
};

const GameType kGameType{
    /*short_name=*/"trictrac",
    /*long_name=*/"Trictrac",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kExplicitStochastic,
    GameType::Information::kPerfectInformation,
    GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    /*min_num_players=*/2,
    /*max_num_players=*/2,
    /*provides_information_state_string=*/false,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/
    {{"schools_enabled", GameParameter(kDefaultSchoolsEnabled)}}
};

static std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new TrictracGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

RegisterSingleTensorObserver single_tensor(kGameType.short_name);

}  // namespace

// Helper methods for move generation
bool TrictracState::CanMoveFrom(int player, int pos) const {
  return board_[player][pos] > 0;
}

bool TrictracState::CanMoveTo(int player, int pos) const {
  return board_[Opponent(player)][pos] <= 1;
}

bool TrictracState::AllCheckersInHomeBoard(int player) const {
  int checkers_count = 0;
  int start_point = (player == kXPlayerId) ? 18 : 0;
  int end_point = (player == kXPlayerId) ? 23 : 5;

  for (int i = 0; i < kNumPoints; ++i) {
    if (i >= start_point && i <= end_point) {
      checkers_count += board_[player][i];
    } else if (board_[player][i] > 0) {
      return false; // Found a checker outside the home board.
    }
  }
  return checkers_count == kNumCheckersPerPlayer;
}

std::vector<CheckerMove> TrictracState::GetPossibleMovesForDie(
    int player, int die) const {
  std::vector<CheckerMove> moves;
  int direction = (player == kXPlayerId) ? 1 : -1;

  for (int from = 0; from < kNumPoints; ++from) {
    if (CanMoveFrom(player, from)) {
      int to = from + die * direction;

      if (to >= 0 && to < kNumPoints) {
        if (CanMoveTo(player, to)) {
          moves.push_back(CheckerMove(from, die, false));
        }
      } else {
        // Bearing off move
        if (AllCheckersInHomeBoard(player)) {
          // TODO: Implement precise bearing off rules (exact roll or furthest checker)
          moves.push_back(CheckerMove(from, die, false));
        }
      }
    }
  }
  return moves;
}

// ============== TrictracState Methods ==============

void TrictracState::SetupInitialBoard() {
  board_[kXPlayerId][0] = 15;
  board_[kOPlayerId][12] = 15;
}

TrictracState::TrictracState(std::shared_ptr<const Game> game,
                                 bool schools_enabled)
    : State(game),
      schools_enabled_(schools_enabled),
      cur_player_(kChancePlayerId),
      prev_player_(kChancePlayerId),
      stage_(GameStage::kPreGame),
      turn_stage_(TurnStage::kRollDice),
      dice_({}),
      board_(
          {std::vector<int>(kNumPoints, 0), std::vector<int>(kNumPoints, 0)}),
      players_(),
      turn_history_info_({}) {
  SetupInitialBoard();
}
Player TrictracState::CurrentPlayer() const {
  return IsTerminal() ? kTerminalPlayerId : Player{cur_player_};
}

int TrictracState::Opponent(int player) const { return 1 - player; }

void TrictracState::RollDice(int outcome) {
  SPIEL_CHECK_TRUE(dice_.empty());
  SetDice(kChanceOutcomeValues[outcome]);
}

void TrictracState::SetDice(const std::vector<int>& dice) {
  dice_ = dice;
  if (dice_[0] >= dice_[1]) {
    std::swap(dice_[0], dice_[1]);
  }
}

int TrictracState::DiceValue(int i) const {
    return dice_[i];
}

void TrictracState::DoApplyAction(Action move) {
  // TODO: Implement Trictrac logic
}

void TrictracState::UndoAction(int player, Action action) {
  // TODO: Implement Trictrac logic
}

bool TrictracState::ApplyCheckerMove(int player, const CheckerMove& move) {
  if (move.pos == kPassPos) return false;

  int direction = (player == kXPlayerId) ? 1 : -1;
  int to = move.pos + move.num * direction;

  board_[player][move.pos]--;
  if (to >= 0 && to < kNumPoints) {
    board_[player][to]++;
  }
  // Note: No score update here as bearing off is just a move, points are separate.
  return false; // No hits in Trictrac
}

void TrictracState::UndoCheckerMove(int player, const CheckerMove& move) {
  if (move.pos == kPassPos) return;

  int direction = (player == kXPlayerId) ? 1 : -1;
  int to = move.pos + move.num * direction;

  board_[player][move.pos]++;
  if (to >= 0 && to < kNumPoints) {
    board_[player][to]--;
  }
}

std::vector<Action> TrictracState::LegalActions() const {
  if (IsChanceNode()) return LegalChanceOutcomes();
  if (IsTerminal()) return {};

  std::vector<Action> actions;
  std::set<std::pair<CheckerMove, CheckerMove>> move_sequences;

  // Use a copy of the state to simulate moves
  TrictracState temp_state = *this;

  int d1 = dice_[0];
  int d2 = dice_[1];

  if (d1 == d2) { // Doubles
    // TODO: Handle 4 moves for doubles. For now, we handle 2.
    auto first_moves = GetPossibleMovesForDie(cur_player_, d1);
    for (const auto& move1 : first_moves) {
      // Apply first move to temp state
      // temp_state.ApplyCheckerMove(cur_player_, move1);
      // auto second_moves = temp_state.GetPossibleMovesForDie(cur_player_, d2);
      // for (const auto& move2 : second_moves) {
      //   move_sequences.insert({move1, move2});
      // }
      // temp_state = *this; // Reset state
    }
  } else { // Not doubles
    std::vector<std::pair<int, int>> dice_orders = {{d1, d2}, {d2, d1}};
    for (const auto& order : dice_orders) {
      auto first_moves = GetPossibleMovesForDie(cur_player_, order.first);
      for (const auto& move1 : first_moves) {
        // TODO: Apply first move to temp state and get second moves
        // For now, let's assume the second move is independent for simplicity
        auto second_moves = GetPossibleMovesForDie(cur_player_, order.second);
        for (const auto& move2 : second_moves) {
           // This is not entirely correct as the board changes, but it's a start
          move_sequences.insert({move1, move2});
        }
      }
    }
  }

  if (move_sequences.empty()) {
    // Pass move
    actions.push_back(0); // TODO: Use a proper pass action encoding
  } else {
    for (const auto& seq : move_sequences) {
      // TODO: Properly encode the sequence into a unique Action
      // actions.push_back(CheckerMovesToSpielMove({seq.first, seq.second}));
    }
  }
  
  // Temporary placeholder until encoding is done
  if (actions.empty()) {
      actions.push_back(0);
  }

  return actions;
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

std::string TrictracState::ActionToString(Player player, Action move_id) const {
    return absl::StrCat("Action: ", move_id);
}

std::string TrictracState::ToString() const {
  std::string board_str = "";
  absl::StrAppend(&board_str, "Stage: ", static_cast<int>(stage_), ", Turn Stage: ", static_cast<int>(turn_stage_), "\n");
  absl::StrAppend(&board_str, "Current Player: ", CurrentPlayer(), "\n");
  absl::StrAppend(&board_str, "Dice: ");
  absl::StrAppend(&board_str, !dice_.empty() ? std::to_string(dice_[0]) : "");
  absl::StrAppend(&board_str, dice_.size() > 1 ? std::to_string(dice_[1]) : "");
  absl::StrAppend(&board_str, "\n");
  absl::StrAppend(&board_str, "Player 0 (X): ", "Points: ", players_[kXPlayerId].points, ", Holes: ", players_[kXPlayerId].holes, "\n");
  absl::StrAppend(&board_str, "Player 1 (O): ", "Points: ", players_[kOPlayerId].points, ", Holes: ", players_[kOPlayerId].holes, "\n");
  return board_str;
}

bool TrictracState::IsTerminal() const {
  return players_[kXPlayerId].holes >= 12 || players_[kOPlayerId].holes >= 12;
}

std::vector<double> TrictracState::Returns() const {
  if (players_[kXPlayerId].holes >= 12) {
    return {1.0, -1.0};
  } else if (players_[kOPlayerId].holes >= 12) {
    return {-1.0, 1.0};
  } else {
    return {0.0, 0.0};
  }
}

std::string TrictracState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  return ToString();
}

void TrictracState::ObservationTensor(Player player, 
                                        absl::Span<float> values) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  int opponent = Opponent(player);
  SPIEL_CHECK_EQ(values.size(), kStateEncodingSize);
  auto value_it = values.begin();
  for (int i = 0; i < kNumPoints; ++i) {
    *value_it++ = board_[player][i];
  }
  for (int i = 0; i < kNumPoints; ++i) {
    *value_it++ = -board_[opponent][i];
  }
  *value_it++ = (cur_player_ == player) ? 1.0 : 0.0;
  *value_it++ = static_cast<float>(turn_stage_);
  *value_it++ = (!dice_.empty()) ? dice_[0] : 0;
  *value_it++ = (dice_.size() > 1) ? dice_[1] : 0;
  *value_it++ = players_[player].points;
  *value_it++ = players_[player].holes;
  *value_it++ = players_[opponent].points;
  *value_it++ = players_[opponent].holes;
  while (value_it != values.end()) {
    *value_it++ = 0;
  }
}

std::unique_ptr<State> TrictracState::Clone() const {
  return std::unique_ptr<State>(new TrictracState(*this));
}

void TrictracState::SetState(int cur_player, const std::vector<int>& dice,
                               const std::vector<std::vector<int>>& board,
                               const std::array<PlayerInfo, 2>& players) {
  cur_player_ = cur_player;
  SetDice(dice);
  board_ = board;
  players_ = players;
}

int TrictracState::board(int player, int pos) const {
    return board_[player][pos];
}

// ============== TrictracGame Methods ============== 

TrictracGame::TrictracGame(const GameParameters& params)
    : Game(kGameType, params),
      schools_enabled_(ParameterValue<bool>("schools_enabled")) {}

}  // namespace trictrac
}  // namespace open_spiel