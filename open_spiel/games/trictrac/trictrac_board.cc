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

#include "open_spiel/games/trictrac/trictrac_board.h"

#include <algorithm>
#include <numeric>

#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace trictrac {

CheckerMove::CheckerMove(int from, int to) : from(from), to(to) {}

CheckerMove CheckerMove::Mirror() const {
  int mirrored_from = (from == 0) ? 0 : 25 - from;
  int mirrored_to = (to == 0) ? 0 : 25 - to;
  return CheckerMove(mirrored_from, mirrored_to);
}

bool CheckerMove::IsExit() const { return to == 0 && from != 0; }

Board::Board() {
  positions_.fill(0);
  positions_[0] = 15;    // White's starting position
  positions_[23] = -15;  // Black's starting position
}

Board Board::Mirror() const {
  Board mirrored_board;
  std::array<int, 24> mirrored_positions;
  for (int i = 0; i < 24; ++i) {
    mirrored_positions[i] = -positions_[23 - i];
  }
  mirrored_board.SetPositions(mirrored_positions);
  return mirrored_board;
}

void Board::MoveChecker(Color color, CheckerMove move) {
  RemoveChecker(color, move.from);
  AddChecker(color, move.to);
}

void Board::AddChecker(Color color, int field) {
  if (field == 0) return;  // Bearing off
  SPIEL_CHECK_GE(field, 1);
  SPIEL_CHECK_LE(field, 24);

  int& pos = positions_[field - 1];
  int unit = (color == Color::kWhite) ? 1 : -1;

  SPIEL_CHECK_TRUE((pos * unit) >= 0);  // Cannot add to a blocked point
  pos += unit;
}

void Board::RemoveChecker(Color color, int field) {
  if (field == 0) return;
  SPIEL_CHECK_GE(field, 1);
  SPIEL_CHECK_LE(field, 24);

  int& pos = positions_[field - 1];
  int unit = (color == Color::kWhite) ? 1 : -1;

  SPIEL_CHECK_TRUE((pos * unit) > 0);  // Must have a checker to remove
  pos -= unit;
}

bool Board::IsBlocked(Color color, int field) const {
  if (field == 0) return false;  // Can always bear off
  SPIEL_CHECK_GE(field, 1);
  SPIEL_CHECK_LE(field, 24);

  int pos_val = positions_[field - 1];
  int unit = (color == Color::kWhite) ? 1 : -1;

  // Blocked if opponent has 2 or more checkers.
  // For this simple check, we consider any opponent checker as a block.
  return (pos_val * unit) < 0;
}

std::vector<std::pair<int, int>> Board::GetPlayerCheckers(
    Color color) const {
  std::vector<std::pair<int, int>> checkers;
  for (int i = 0; i < 24; ++i) {
    int count = positions_[i];
    if (color == Color::kWhite && count > 0) {
      checkers.push_back({i + 1, count});
    } else if (color == Color::kBlack && count < 0) {
      checkers.push_back({i + 1, -count});
    }
  }
  return checkers;
}

bool Board::AnyQuarterFilled(Color color) const {
  return IsQuarterFilled(color, 1) || IsQuarterFilled(color, 7) ||
         IsQuarterFilled(color, 13) || IsQuarterFilled(color, 19);
}

bool Board::IsQuarterFilled(Color color, int field) const {
  std::array<int, 6> quarter = GetQuarterFields(field);
  for (int f : quarter) {
    int count = positions_[f - 1];
    if (color == Color::kWhite && count < 2) return false;
    if (color == Color::kBlack && count > -2) return false;
  }
  return true;
}

bool Board::IsQuarterFillable(Color color, int field) const {
  std::array<int, 6> quarter = GetQuarterFields(field);
  int unit = (color == Color::kWhite) ? 1 : -1;

  // Check for opponent's checkers
  for (int f : quarter) {
    if (positions_[f - 1] * unit < 0) return false;
  }

  // Simplified check: just ensure no opponent checkers are present.
  // A full implementation would check if enough checkers are available
  // to fill the remaining spots.
  return true;
}

std::array<int, 6> Board::GetQuarterFields(int field) const {
  SPIEL_CHECK_GE(field, 1);
  SPIEL_CHECK_LE(field, 24);
  int start = 1 + (((field - 1) / 6) * 6);
  std::array<int, 6> quarter;
  std::iota(quarter.begin(), quarter.end(), start);
  return quarter;
}

#include <sstream>
#include <iomanip>
#include <vector>

namespace open_spiel {
namespace trictrac {

// Helper function to transpose a 2D vector of strings.
std::vector<std::vector<std::string>> Transpose(
    std::vector<std::vector<std::string>>& matrix) {
  if (matrix.empty() || matrix[0].empty()) {
    return {};
  }
  std::vector<std::vector<std::string>> transposed(
      matrix[0].size(), std::vector<std::string>(matrix.size()));
  for (size_t i = 0; i < matrix.size(); ++i) {
    for (size_t j = 0; j < matrix[0].size(); ++j) {
      transposed[j][i] = matrix[i][j];
    }
  }
  return transposed;
}


std::string Board::ToString() const {
    const int kColSize = 5;
    std::vector<std::vector<std::string>> columns;
    columns.reserve(24);

    for (int count : positions_) {
        std::string checker_char = (count > 0) ? "O" : "X";
        int num_checkers = std::abs(count);
        std::vector<std::string> cells(kColSize, " ");
        for (int i = 0; i < std::min(num_checkers, kColSize); ++i) {
            cells[i] = checker_char;
        }
        if (num_checkers > kColSize) {
            cells[kColSize - 1] = std::to_string(num_checkers);
        }
        columns.push_back(cells);
    }

    std::vector<std::vector<std::string>> upper_positions(columns.begin() + 12, columns.end());
    std::reverse(upper_positions.begin(), upper_positions.end());

    std::vector<std::vector<std::string>> lower_positions(columns.begin(), columns.begin() + 12);
     for (auto& col : lower_positions) {
        std::reverse(col.begin(), col.end());
    }
    std::reverse(lower_positions.begin(), lower_positions.end());


    std::stringstream ss;
    ss << "     13   14   15   16   17   18      19   20   21   22   23   24\n";
    ss << "  ----------------------------------------------------------------\n";

    auto upper_transposed = Transpose(upper_positions);
    for (const auto& row : upper_transposed) {
        ss << " |";
        for (int i = 0; i < 6; ++i) ss << std::setw(5) << row[i];
        ss << " | |";
        for (int i = 6; i < 12; ++i) ss << std::setw(5) << row[i];
        ss << " |\n";
    }

    ss << " |------------------------------ | | -----------------------------|\n";

    auto lower_transposed = Transpose(lower_positions);
    for (const auto& row : lower_transposed) {
        ss << " |";
        for (int i = 0; i < 6; ++i) ss << std::setw(5) << row[i];
        ss << " | |";
        for (int i = 6; i < 12; ++i) ss << std::setw(5) << row[i];
        ss << " |\n";
    }

    ss << "  ----------------------------------------------------------------\n";
    ss << "    12   11   10    9    8    7        6    5    4    3    2    1\n";

    return ss.str();
}

void Board::SetPositions(const std::array<int, 24>& positions) {
  positions_ = positions;
}


}  // namespace trictrac
}  // namespace open_spiel
