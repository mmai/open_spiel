clean:
  rm -rf build
build:
  CXX=$(which clang++) bash ./open_spiel/scripts/build_and_run_tests.sh
[working-directory: 'build']
buildshared:
  BUILD_SHARED_LIB=ON CXX=$(which clang++) cmake -DPython3_EXECUTABLE=$(which python3) -DCMAKE_CXX_COMPILER=${CXX} ../open_spiel
  make -j$(nproc) open_spiel
play game:
  build/examples/example --game={{game}}
playpython game:
  PYTHONPATH=$PYTHONPATH:./.:./build/python python3 open_spiel/python/examples/example.py --game_string={{game}}
[working-directory: 'build']
buildshared:
  BUILD_SHARED_LIB=ON CXX=$(which clang++) cmake -DPython3_EXECUTABLE=$(which python3) -DCMAKE_CXX_COMPILER=${CXX} ../open_spiel
  make -j$(nproc) open_spiel
[working-directory: 'open_spiel/examples']
buildsharedgames:
  clang++ -I${HOME}/open_spiel -I${HOME}/open_spiel/open_spiel/abseil-cpp \
        -std=c++17 -o shared_library_example shared_library_example.cc \
        -L${HOME}/open_spiel/build  -lopen_spiel
