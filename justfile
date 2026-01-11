install:
  # libnop
  git clone -b 'master' --single-branch --depth 1  https://github.com/google/libnop.git open_spiel/libnop/libnop
  # libtorch. get correct download url here : https://pytorch.org/get-started/locally/
  wget --show-progress -O libtorch.zip "https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-2.9.1%2Bcpu.zip"
  unzip libtorch.zip -d open_spiel/libtorch/
# build: install
build:
  CXX=$(which clang++) bash ./open_spiel/scripts/build_and_run_tests.sh
play game:
  build/examples/example --game={{game}}
playpython game:
  python3 open_spiel/python/examples/example.py --game_string={{game}}
trainzero game:
  ./build/examples/alpha_zero_torch_example --game={{game}} --path=./trainzero/{{game}}/
continuetrainzero game:
  ./build/examples/alpha_zero_torch_example ./trainzero/{{game}}/config.json
playzero game:
  #./build/examples/alpha_zero_torch_game_example --game={{game}} --player1=az --player2=mcts --az_path=./trainzero/{{game}}/ --az_checkpoint=-1
  ./build/examples/alpha_zero_torch_game_example --game={{game}} --player1=az --player2=random --az_path=./trainzero/{{game}}/ --az_checkpoint=-1
