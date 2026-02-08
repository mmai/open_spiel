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
"""Trictrac implemented in Python.
"""

import enum
import numpy as np
import pyspiel
import trictrac_store

# for Open Spiel game info
_NUM_PLAYERS = 2
_NUM_CHANCE_OUTCOMES = 36 # 2 dice
# Action space size from Rust implementation (bot/src/training_common.rs)
# 1 (Roll) + 1 (Go) + 512 (mouvements possibles)
_NUM_DISTINCT_ACTIONS = 514

# State encoding size from Rust implementation (store/src/game.rs)
# 24 (board) + 1 (active player) + 1 (turn stage) + 2 (dice) + 8 (players stats)
_STATE_ENCODING_SIZE = 36

_DEFAULT_MAX_PLAYER_TURNS = 1000 # Adjusted estimate

# _NUM_POINTS = 24
# _NUM_DICE_OUTCOMES = 6
# _NUM_CHECKERS_PER_PLAYER = 15

_GAME_TYPE = pyspiel.GameType(
    short_name="python_trictrac",
    long_name="Python Trictrac",
    dynamics=pyspiel.GameType.Dynamics.SEQUENTIAL,
    chance_mode=pyspiel.GameType.ChanceMode.EXPLICIT_STOCHASTIC,
    information=pyspiel.GameType.Information.PERFECT_INFORMATION,
    # utility=pyspiel.GameType.Utility.ZERO_SUM,
    # reward_model=pyspiel.GameType.RewardModel.TERMINAL,
    utility=pyspiel.GameType.Utility.GENERAL_SUM,
    reward_model=pyspiel.GameType.RewardModel.REWARDS,
    max_num_players=_NUM_PLAYERS,
    min_num_players=_NUM_PLAYERS,
    provides_information_state_string=False,
    provides_information_state_tensor=False,
    provides_observation_string=True,
    provides_observation_tensor=True,
    parameter_specification={
        "max_player_turns": _DEFAULT_MAX_PLAYER_TURNS
    })

_GAME_INFO = pyspiel.GameInfo(
    num_distinct_actions=_NUM_DISTINCT_ACTIONS,
    max_chance_outcomes=_NUM_CHANCE_OUTCOMES,
    num_players=_NUM_PLAYERS,
    # min_utility=-1.0,
    # max_utility=1.0,
    # utility_sum=0.0,
    min_utility=0.0,
    max_utility=180.0, # 12 points * 12 holes = 144 + last play possible points = ??
    max_game_length=3 * _DEFAULT_MAX_PLAYER_TURNS) # Rough estimate


class TrictracGame(pyspiel.Game):
  """A Python version of the Trictrac game."""

  def __init__(self, params=None):
    super().__init__(_GAME_TYPE, _GAME_INFO, params or dict())

  def new_initial_state(self):
    """Returns a state corresponding to the start of a game."""
    return TrictracState(self)

  # def max_utility(self):
  #   """Returns the maximum possible utility in the game."""
  #   pass

  def observation_tensor_shape(self):
    """Returns the shape of the observation tensor."""
    return [_STATE_ENCODING_SIZE]

class TrictracState(pyspiel.State):
  """A python version of the Trictrac state."""

  def __init__(self, game):
    """Constructor; should only be called by Game.new_initial_state."""
    self._store = trictrac_store.TricTrac()
    super().__init__(game)

  def is_chance_node(self):
    # print("chance node ? ", self._store.needs_roll())
    return self._store.needs_roll()

  def current_player(self):
    """Returns id of the current player to act.

    The id is:
      - TERMINAL if game is over.
      - CHANCE if a player is drawing a number to fill out their hand.
      - a number otherwise.
    """
    if self.is_terminal():
      return pyspiel.PlayerId.TERMINAL
    elif self._store.needs_roll():
      return pyspiel.PlayerId.CHANCE
    else:
      return self._store.current_player_idx()

  def _legal_actions(self, player):
    """Returns a list of legal actions, sorted in ascending order."""
    return self._store.get_legal_actions(player)

  def _roll_from_chance_idx(self, action):
    return [(i,j) for i in range(1,7) for j in range(1,7)][action]

  def _action_to_string(self, player, action):
    """Action -> string."""
    if self.is_chance_node():
        return "{}".format(self._roll_from_chance_idx(action))
    else:
        return self._store.action_to_string(player, action)

  def chance_outcomes(self):
    """Returns the possible chance outcomes and their probabilities."""
    assert self.is_chance_node()
    p = 1.0 / _NUM_CHANCE_OUTCOMES
    return [(i, p) for i in range(0,36)]

  def _apply_action(self, action):
    """Applies the specified action to the state."""
    print("in apply action", self.is_chance_node(), action)
    if self.is_chance_node():
        self._store.apply_dice_roll(self._roll_from_chance_idx(action))
    else:
        self._store.apply_action(action)

  def is_terminal(self):
    """Returns True if the game is over."""
    return self._store.is_game_ended()

  def returns(self):
    """Total reward for each player over the course of the game so far."""
    return self._store.get_players_scores()

  def observation_string(self, player):
    """Returns a string representation of the observation for the player."""
    self._store.get_observation_string(player)

  def observation_tensor(self, player, values):
    """Populates the observation tensor for the player."""
    self._store.get_tensor(player)

  def __str__(self):
    """String for debug purposes. No particular semantics are required."""
    return ""

pyspiel.register_game(_GAME_TYPE, TrictracGame)
