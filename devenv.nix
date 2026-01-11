{ pkgs, lib, config, inputs, ... }:

{

  languages.cplusplus.enable = true;
  languages.python =
    {
      enable = true;
      venv.enable = true;
      venv.requirements = ./requirements.txt;
    };

  # PYTHONPATH=$PYTHONPATH:$PWD/.devenv/state/venv/lib/python3/site-packages
  enterShell = ''
    PYTHONPATH=$PYTHONPATH:$PWD:$PWD/build/python
  '';

  packages = [
    pkgs.clang
  ];
}
