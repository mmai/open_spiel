{ pkgs, lib, config, inputs, ... }:

{

  languages.cplusplus.enable = true;
  languages.python =
    {
      enable = true;
      venv.enable = true;
      venv.requirements = ./requirements.txt;
    };

  packages = [
    pkgs.clang
  ];
}
