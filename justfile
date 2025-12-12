build:
  CXX=$(which clang++) bash ./open_spiel/scripts/build_and_run_tests.sh
play game:
  build/examples/example --game={{game}}
playpython game:
  PYTHONPATH=$PYTHONPATH:./.:./build/python python3 open_spiel/python/examples/example.py --game_string={{game}}
