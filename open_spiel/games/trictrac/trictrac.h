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

#ifndef OPEN_SPIEL_GAMES_TRICTRAC_H_
#define OPEN_SPIEL_GAMES_TRICTRAC_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "open_spiel/abseil-cpp/absl/types/optional.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"

// Trictrac implementation.
//
// Parameters:
//   "schools_enabled"  bool    Whether to enable schools (def: false)

namespace open_spiel {
namespace trictrac {

inline constexpr const int kNumPlayers = 2;
inline constexpr const int kNumChanceOutcomes = 36;
inline constexpr const int kNumPoints = 24;
inline constexpr const int kNumDiceOutcomes = 6;
inline constexpr const int kXPlayerId = 0;
inline constexpr const int kOPlayerId = 1;
inline constexpr const int kPassPos = -1;
inline constexpr const int kNumCheckersPerPlayer = 15;
inline constexpr const int kBarPos = 100;
inline constexpr const int kScorePos = 101;
inline constexpr const int kNumDistinctActions = 1352;
inline constexpr const int kBoardEncodingSize = 4 * kNumPoints * kNumPlayers;
inline constexpr const int kStateEncodingSize =
    3 * kNumPlayers + kBoardEncodingSize + 2;
inline constexpr bool kDefaultSchoolsEnabled = false;

enum class GameStage {
  kPreGame,
  kInGame,
  kEnded,
};

enum class TurnStage {
  kRollDice,
  kRollWaiting,
  kMarkPoints,
  kHoldOrGoChoice,
  kMove,
  kMarkAdvPoints,
};

struct PlayerInfo {
  int points = 0;
  int holes = 0;
  bool can_bredouille = true;
  bool can_big_bredouille = true;
  int dice_roll_count = 0;
};

struct CheckerMove {
  // Pass is encoded as (pos, num, hit) = (-1, -1, false).
  int pos;  // 0-24  (0-23 for locations on the board and kBarPos)
  int num;  // 1-6
  bool hit;
  CheckerMove(int _pos, int _num, bool _hit)
      : pos(_pos), num(_num), hit(_hit) {}
  bool operator<(const CheckerMove& rhs) const {
    return (pos * 6 + (num - 1)) < (rhs.pos * 6 + rhs.num - 1);
  }
};

// This is a small helper to track historical turn info not stored in the moves.
// It is only needed for proper implementation of Undo.
struct TurnHistoryInfo {
  int player;
  int prev_player;
  std::vector<int> dice;
  Action action;
  bool double_turn;
  bool first_move_hit;
  bool second_move_hit;
  TurnHistoryInfo(int _player, int _prev_player, std::vector<int> _dice,
                  int _action, bool _double_turn, bool fmh, bool smh)
      : player(_player),
        prev_player(_prev_player),
        dice(_dice),
        action(_action),
        double_turn(_double_turn),
        first_move_hit(fmh),
        second_move_hit(smh) {}
};

class TrictracGame;

class TrictracState : public State {
 public:
  TrictracState(const TrictracState&) = default;
  TrictracState(std::shared_ptr<const Game>, bool schools_enabled);

  Player CurrentPlayer() const override;
  void UndoAction(Player player, Action action) override;
  std::vector<Action> LegalActions() const override;
  std::string ActionToString(Player player, Action move_id) const override;
  std::vector<std::pair<Action, double>> ChanceOutcomes() const override;
  std::string ToString() const override;
  bool IsTerminal() const override;
  std::vector<double> Returns() const override;
  std::string ObservationString(Player player) const override;
  void ObservationTensor(Player player,
                         absl::Span<float> values) const override;
  std::unique_ptr<State> Clone() const override;

  void SetState(int cur_player, const std::vector<int>& dice,
                const std::vector<std::vector<int>>& board,
                const std::array<PlayerInfo, 2>& players);

  int Opponent(int player) const;
  int GetDistance(int player, int from, int to) const;
  bool IsOff(int player, int pos) const;
  bool IsFurther(int player, int pos1, int pos2) const;
  bool IsLegalFromTo(int player, int from_pos, int to_pos, int my_checkers_from,
                     int opp_checkers_to) const;
  int GetToPos(int player, int from_pos, int pips) const;
  int CountTotalCheckers(int player) const;
  bool IsHit(Player player, int from_pos, int num) const;

  int board(int player, int pos) const;
  Action CheckerMovesToSpielMove(const std::vector<CheckerMove>& moves) const;
  std::vector<CheckerMove> SpielMoveToCheckerMoves(int player,
                                                   Action spiel_move) const;
  Action TranslateAction(int from1, int from2, bool use_high_die_first) const;
  std::vector<CheckerMove>
  AugmentWithHitInfo(Player player,
                     const std::vector<CheckerMove> &cmoves) const;

 protected:
  void DoApplyAction(Action move_id) override;

 private:
  // Helper methods for move generation
  bool CanMoveFrom(int player, int pos) const;
  bool CanMoveTo(int player, int pos) const;
  bool AllCheckersInHomeBoard(int player) const;
  std::vector<CheckerMove> GetPossibleMovesForDie(int player, int die) const;

  void SetupInitialBoard();
  void RollDice(int outcome);
  void SetDice(const std::vector<int>& dice);
  bool IsPosInHome(int player, int pos) const;
  bool AllInHome(int player) const;
  int CheckersInHome(int player) const;
  bool UsableDiceOutcome(int outcome) const;
  int PositionFromBar(int player, int spaces) const;
  int PositionFrom(int player, int pos, int spaces) const;
  int NumOppCheckers(int player, int pos) const;
  std::string DiceToString(int outcome) const;
  int DiceValue(int i) const;
  int HighestUsableDiceOutcome() const;
  Action EncodedPassMove() const;
  Action EncodedBarMove() const;
  int AugmentCheckerMove(CheckerMove* cmove, int player, int start) const;
  int FurthestCheckerInHome(int player) const;
  bool ApplyCheckerMove(int player, const CheckerMove& move);
  void UndoCheckerMove(int player, const CheckerMove& move);
  std::set<CheckerMove> LegalCheckerMoves(int player) const;
  int RecLegalMoves(std::vector<CheckerMove> moveseq,
                    std::set<std::vector<CheckerMove>>* movelist);
  std::vector<Action> ProcessLegalMoves(
      int max_moves, const std::set<std::vector<CheckerMove>>& movelist) const;

  bool schools_enabled_;

  Player cur_player_;
  Player prev_player_;
  GameStage stage_;
  TurnStage turn_stage_;
  std::vector<int> dice_;
  std::vector<std::vector<int>> board_;
  std::array<PlayerInfo, 2> players_;
  std::vector<TurnHistoryInfo> turn_history_info_;
};

class TrictracGame : public Game {
 public:
  explicit TrictracGame(const GameParameters& params);

  int NumDistinctActions() const override { return kNumDistinctActions; }

  std::unique_ptr<State> NewInitialState() const override {
    return std::unique_ptr<State>(new TrictracState(
        shared_from_this(), schools_enabled_));
  }

  int MaxChanceOutcomes() const override { return 36; }
  int MaxGameLength() const override { return 1000; }
  int MaxChanceNodesInHistory() const override { return MaxGameLength() + 1; }
  int NumPlayers() const override { return 2; }
  double MinUtility() const override { return -1; }
  absl::optional<double> UtilitySum() const override { return 0; }
  double MaxUtility() const override { return 1; }

  std::vector<int> ObservationTensorShape() const override {
    return {kStateEncodingSize};
  }

  int NumCheckersPerPlayer() const { return kNumCheckersPerPlayer; }

 private:
  bool schools_enabled_;
};

}  // namespace trictrac
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_TRICTRAC_H_
