# Copyright 2019 DeepMind Technologies Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Lint as python3
"""Backgammon implemented in Python.

This is a template for the Python implementation of Backgammon.
"""

import enum
import numpy as np
import pyspiel

_NUM_PLAYERS = 2
_NUM_CHANCE_OUTCOMES = 36
_NUM_POINTS = 24
_NUM_DICE_OUTCOMES = 6
_X_PLAYER_ID = 0
_O_PLAYER_ID = 1
_PASS_POS = -1
_NUM_CHECKERS_PER_PLAYER = 15
_BAR_POS = 100
_SCORE_POS = 101
_NUM_DISTINCT_ACTIONS = 1352

_BOARD_ENCODING_SIZE = 4 * _NUM_POINTS * _NUM_PLAYERS
_STATE_ENCODING_SIZE = 3 * _NUM_PLAYERS + _BOARD_ENCODING_SIZE + 2

_DEFAULT_SCORING_TYPE = "winloss_scoring"
_DEFAULT_HYPER_BACKGAMMON = False
_DEFAULT_MAX_PLAYER_TURNS = 500


class ScoringType(enum.Enum):
  WINLOSS_SCORING = 0
  ENABLE_GAMMONS = 1
  FULL_SCORING = 2


_GAME_TYPE = pyspiel.GameType(
    short_name="python_backgammon",
    long_name="Python Backgammon",
    dynamics=pyspiel.GameType.Dynamics.SEQUENTIAL,
    chance_mode=pyspiel.GameType.ChanceMode.EXPLICIT_STOCHASTIC,
    information=pyspiel.GameType.Information.PERFECT_INFORMATION,
    utility=pyspiel.GameType.Utility.ZERO_SUM,
    reward_model=pyspiel.GameType.RewardModel.TERMINAL,
    max_num_players=_NUM_PLAYERS,
    min_num_players=_NUM_PLAYERS,
    provides_information_state_string=False,
    provides_information_state_tensor=False,
    provides_observation_string=True,
    provides_observation_tensor=True,
    parameter_specification={
        "hyper_backgammon": _DEFAULT_HYPER_BACKGAMMON,
        "scoring_type": _DEFAULT_SCORING_TYPE,
        "max_player_turns": _DEFAULT_MAX_PLAYER_TURNS
    })

_GAME_INFO = pyspiel.GameInfo(
    num_distinct_actions=_NUM_DISTINCT_ACTIONS,
    max_chance_outcomes=_NUM_CHANCE_OUTCOMES,
    num_players=_NUM_PLAYERS,
    min_utility=-3.0,
    max_utility=3.0,
    utility_sum=0.0,
    max_game_length=3 * _DEFAULT_MAX_PLAYER_TURNS)


class CheckerMove:
  """A helper class to track checker moves."""

  def __init__(self, pos, num, hit):
    self.pos = pos
    self.num = num
    self.hit = hit


class BackgammonGame(pyspiel.Game):
  """A Python version of the Backgammon game."""

  def __init__(self, params=None):
    super().__init__(_GAME_TYPE, _GAME_INFO, params or dict())

  def new_initial_state(self):
    """Returns a state corresponding to the start of a game."""
    return BackgammonState(self)

  def max_utility(self):
    """Returns the maximum possible utility in the game."""
    pass

  def observation_tensor_shape(self):
    """Returns the shape of the observation tensor."""
    return [_STATE_ENCODING_SIZE]

  def num_checkers_per_player(self):
    """Returns the number of checkers per player."""
    pass


class BackgammonState(pyspiel.State):
  """A python version of the Backgammon state."""

  def __init__(self, game):
    """Constructor; should only be called by Game.new_initial_state."""
    super().__init__(game)

  def current_player(self):
    """Returns id of the next player to move, or TERMINAL if game is over."""
    pass

  def _legal_actions(self, player):
    """Returns a list of legal actions, sorted in ascending order."""
    pass

  def _action_to_string(self, player, action):
    """Action -> string."""
    pass

  def chance_outcomes(self):
    """Returns the possible chance outcomes and their probabilities."""
    pass

  def _apply_action(self, action):
    """Applies the specified action to the state."""
    pass

  def _undo_action(self, player, action):
    """Undoes the specified action."""
    pass

  def is_terminal(self):
    """Returns True if the game is over."""
    pass

  def returns(self):
    """Total reward for each player over the course of the game so far."""
    pass

  def observation_string(self, player):
    """Returns a string representation of the observation for the player."""
    pass

  def observation_tensor(self, player, values):
    """Populates the observation tensor for the player."""
    pass

  def __str__(self):
    """String for debug purposes. No particular semantics are required."""
    pass

  def set_state(self, cur_player, double_turn, dice, bar, scores, board):
    """Setter function used for debugging and tests."""
    pass

  def opponent(self, player):
    """Returns the opponent of the specified player."""
    pass

  def get_distance(self, player, from_pos, to_pos):
    """Compute a distance between 'from' and 'to'."""
    pass

  def is_off(self, player, pos):
    """Is this position off the board?"""
    pass

  def is_further(self, player, pos1, pos2):
    """Returns whether pos2 is further than pos1 for the player."""
    pass

  def is_legal_from_to(self, player, from_pos, to_pos, my_checkers_from,
                       opp_checkers_to):
    """Is this a legal from -> to checker move?"""
    pass

  def get_to_pos(self, player, from_pos, pips):
    """Get the To position for this play."""
    pass

  def count_total_checkers(self, player):
    """Count the total number of checkers for this player."""
    pass

  def is_hit(self, player, from_pos, num):
    """Returns if moving from the position for the number of spaces is a hit."""
    pass

  def board(self, player, pos):
    """Get the number of checkers on the board in the specified position."""
    pass

  def checker_moves_to_spiel_move(self, moves):
    """Action encoding function."""
    pass

  def spiel_move_to_checker_moves(self, player, spiel_move):
    """Action decoding function."""
    pass

  def translate_action(self, from1, from2, use_high_die_first):
    """Translates individual moves into a single action."""
    pass

  def augment_with_hit_info(self, player, cmoves):
    """Return checker moves with extra hit information."""
    pass


pyspiel.register_game(_GAME_TYPE, BackgammonGame)
