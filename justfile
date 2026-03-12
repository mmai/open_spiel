install:
  # libnop
  git clone -b 'master' --single-branch --depth 1  https://github.com/google/libnop.git open_spiel/libnop/libnop
  # libtorch. get correct download url here : https://pytorch.org/get-started/locally/
  wget --show-progress -O libtorch.zip "https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-2.9.1%2Bcpu.zip"
  unzip libtorch.zip -d open_spiel/libtorch/
# build: install
clean:
  rm -rf build
build:
  CXX=$(which clang++) bash ./open_spiel/scripts/build_and_run_tests.sh
play game:
  build/examples/example --game={{game}}
playpython game:
  python3 open_spiel/python/examples/example.py --game_string={{game}}
installtrictrac:
  pip install --no-deps --force-reinstall --prefix .devenv/state/venv ../../trictrac/target/wheels/*.whl
buildtrictrac:
  mkdir -p build
  cd build && CXX=$(which clang++) cmake ../open_spiel && make -j$(nproc) trictrac_test
  # cd build && CXX=$(which clang++) cmake -DCMAKE_BUILD_TYPE=Release ../open_spiel && make -j$(nproc) trictrac_test
testtrictrac: buildtrictrac
  ./build/games/trictrac_test
traintrictrac:
  python3 open_spiel/python/examples/trictrac_ppo.py
playtrictrac:
  python3 open_spiel/python/examples/example.py --game_string=python_trictrac
playtrictrac_cpp:
  ./build/examples/example --game=trictrac
buildzero:
  mkdir -p build
  cd build && OPEN_SPIEL_BUILD_WITH_LIBTORCH=ON OPEN_SPIEL_BUILD_WITH_LIBNOP=ON CXX=$(which clang++) cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native" ../open_spiel && make -j$(nproc) alpha_zero_torch_example
buildzero_debug:
  mkdir -p build
  cd build && OPEN_SPIEL_BUILD_WITH_LIBTORCH=ON OPEN_SPIEL_BUILD_WITH_LIBNOP=ON CXX=$(which clang++) cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../open_spiel && make -j$(nproc) alpha_zero_torch_example
trainzero game:
  ./build/examples/alpha_zero_torch_example --game={{game}} --path=./trainzero/{{game}}
trainzeromlp game:
  ./build/examples/alpha_zero_torch_example --game={{game}} --path=./trainzero/{{game}} --nn_model=mlp --max_simulations=10
continuetrainzero game:
  ./build/examples/alpha_zero_torch_example ./trainzero/{{game}}/config.json
playzero game:
  #./build/examples/alpha_zero_torch_game_example --game={{game}} --player1=az --player2=mcts --az_path=./trainzero/{{game}}/ --az_checkpoint=-1
  ./build/examples/alpha_zero_torch_game_example --game={{game}} --player1=az --player2=random --az_path=./trainzero/{{game}}/ --az_checkpoint=-1
[working-directory: 'build']
buildshared:
  BUILD_SHARED_LIB=ON CXX=$(which clang++) cmake -DPython3_EXECUTABLE=$(which python3) -DCMAKE_CXX_COMPILER=${CXX} ../open_spiel
  make -j$(nproc) open_spiel
[working-directory: 'open_spiel/examples']
buildsharedgames:
  clang++ -I${HOME}/open_spiel -I${HOME}/open_spiel/open_spiel/abseil-cpp \
        -std=c++17 -o shared_library_example shared_library_example.cc \
        -L${HOME}/open_spiel/build  -lopen_spiel
