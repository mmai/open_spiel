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

#include <iostream>
#include <memory>
#include <string>

#include "open_spiel/spiel.h"
#include "open_spiel/tests/basic_tests.h"
#include "open_spiel/utils/init.h"

namespace open_spiel {
namespace trictrac {
namespace {

void BasicTests() {
  testing::LoadGameTest("trictrac");
  testing::RandomSimTest(*LoadGame("trictrac"), /*num_sims=*/3);
}

}  // namespace
}  // namespace trictrac
}  // namespace open_spiel

int main(int argc, char** argv) {
  open_spiel::Init(argv[0], &argc, &argv, true);
  open_spiel::trictrac::BasicTests();
  std::cout << "trictrac_test: all tests passed." << std::endl;
  return 0;
}
