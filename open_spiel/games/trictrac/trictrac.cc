// Copyright 2024 Henri Rebecq
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
#include <memory>
#include <string>
#include <vector>

#include "rust/cxx.h"
#include "open_spiel/abseil-cpp/absl/types/span.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_globals.h"
#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace trictrac {
namespace {

// ── Game type registration ────────────────────────────────────────────────────

const GameType kGameType{
    /*short_name=*/"trictrac",
    /*long_name=*/"Trictrac",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kExplicitStochastic,
    GameType::Information::kPerfectInformation, GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    // GameType::RewardModel::kRewards,
    /*min_num_players=*/2,
    /*max_num_players=*/2,
    /*provides_information_state_string=*/false,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/{
        {"max_turns", GameParameter(kDefaultMaxTurns)},
    }};

static std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::make_shared<const TrictracGame>(params);
}

REGISTER_SPIEL_GAME(kGameType, Factory);

}  // namespace

// ── TrictracGame ─────────────────────────────────────────────────────────────

TrictracGame::TrictracGame(const GameParameters& params)
    : Game(kGameType, params),
      max_turns_(ParameterValue<int>("max_turns", kDefaultMaxTurns)) {}

std::unique_ptr<State> TrictracGame::NewInitialState() const {
  return std::make_unique<TrictracState>(shared_from_this());
}

// ── TrictracState ─────────────────────────────────────────────────────────────

TrictracState::TrictracState(std::shared_ptr<const Game> game)
    : State(game), engine_(trictrac_engine::new_trictrac_engine()) {}

TrictracState::TrictracState(const TrictracState& other)
    : State(other), engine_(other.engine_->clone_engine()) {}

std::unique_ptr<State> TrictracState::Clone() const {
  return std::make_unique<TrictracState>(*this);
}

// ── Current player ────────────────────────────────────────────────────────────

Player TrictracState::CurrentPlayer() const {
  if (engine_->is_game_ended()) return kTerminalPlayerId;
  if (engine_->needs_roll()) return kChancePlayerId;
  return static_cast<Player>(engine_->current_player_idx());
}

// ── Legal actions ─────────────────────────────────────────────────────────────

std::vector<Action> TrictracState::LegalActions() const {
  if (IsChanceNode()) {
    std::vector<Action> outcomes(kNumChanceOutcomes);
    for (int i = 0; i < kNumChanceOutcomes; ++i) outcomes[i] = i;
    return outcomes;
  }
  const rust::Vec<uint64_t> rust_actions =
      engine_->get_legal_actions(static_cast<uint64_t>(CurrentPlayer()));
  std::vector<Action> actions;
  actions.reserve(rust_actions.size());
  for (uint64_t a : rust_actions) actions.push_back(static_cast<Action>(a));
  std::sort(actions.begin(), actions.end());
  actions.erase(std::unique(actions.begin(), actions.end()), actions.end());
  return actions;
}

// ── Chance outcomes ───────────────────────────────────────────────────────────

std::vector<std::pair<Action, double>> TrictracState::ChanceOutcomes() const {
  SPIEL_CHECK_TRUE(IsChanceNode());
  const double p = 1.0 / kNumChanceOutcomes;
  std::vector<std::pair<Action, double>> outcomes;
  outcomes.reserve(kNumChanceOutcomes);
  for (int i = 0; i < kNumChanceOutcomes; ++i) outcomes.emplace_back(i, p);
  return outcomes;
}

// ── Apply action ──────────────────────────────────────────────────────────────

/*static*/
trictrac_engine::DicePair TrictracState::DecodeChanceAction(Action action) {
  // Matches: [(i,j) for i in range(1,7) for j in range(1,7)][action]
  return trictrac_engine::DicePair{
      static_cast<uint8_t>(action / 6 + 1),
      static_cast<uint8_t>(action % 6 + 1),
  };
}

void TrictracState::DoApplyAction(Action action) {
  try {
    if (IsChanceNode()) {
      engine_->apply_dice_roll(DecodeChanceAction(action));
    } else {
      engine_->apply_action(static_cast<uint64_t>(action));
    }
  } catch (const rust::Error& e) {
    SpielFatalError(absl::StrCat("TrictracState::DoApplyAction failed: ", e.what()));
  }
}

// ── Terminal & returns ────────────────────────────────────────────────────────

bool TrictracState::IsTerminal() const {
  return engine_->is_game_ended();
}

std::vector<double> TrictracState::Returns() const {
  if (!IsTerminal()) return {0.0, 0.0};
  const trictrac_engine::PlayerScores scores = engine_->get_players_scores();
  double diff = static_cast<double>(scores.score_p1) - static_cast<double>(scores.score_p2);
  return {diff, -diff};
}

// ── Observation ───────────────────────────────────────────────────────────────

std::string TrictracState::ObservationString(Player player) const {
  return std::string(engine_->get_observation_string(
      static_cast<uint64_t>(player)));
}

void TrictracState::ObservationTensor(Player player,
                                      absl::Span<float> values) const {
  SPIEL_CHECK_EQ(static_cast<int>(values.size()), kStateEncodingSize);
  const rust::Vec<int8_t> tensor =
      engine_->get_tensor(static_cast<uint64_t>(player));
  SPIEL_CHECK_EQ(tensor.size(), static_cast<size_t>(kStateEncodingSize));
  for (int i = 0; i < kStateEncodingSize; ++i) {
    values[i] = static_cast<float>(tensor[i]);
  }
}

// ── Strings ───────────────────────────────────────────────────────────────────

std::string TrictracState::ToString() const {
  return std::string(engine_->to_debug_string());
}

std::string TrictracState::ActionToString(Player player, Action action) const {
  if (IsChanceNode()) {
    const trictrac_engine::DicePair d = DecodeChanceAction(action);
    return "(" + std::to_string(d.die1) + ", " + std::to_string(d.die2) + ")";
  }
  return std::string(engine_->action_to_string(
      static_cast<uint64_t>(player), static_cast<uint64_t>(action)));
}

}  // namespace trictrac
}  // namespace open_spiel
