# TODO

## But du projet

Le dossier forks/open_spiel contient mon fork du projet Open Spiel de Google DeepMind. J'ai créé un nouveau dossier open_spiel/open_spiel/games/trictrac. Je veux donc maintenant implémenter le jeu de Trictrac pour pouvoir entrainer un agent avec les algorithmes disponibles dans Open Spiel. Le principe est d'adapter les classes TrictracGame et TrictracState pour respecter les règles du Trictrac.

J'ai déjà fait une implémentation du jeu de Trictrac en rust dans le dossier ./trictrac/ et je voudrais m'en inspirer pour faire la version c++ pouvant être intégrée dans Open Spiel.

## Structure du projet rust

Les règles du jeu et l'état d'une partie sont implémentées dans 'trictrac/store'.
Plus précisément, l'état du jeu est défini par le struct GameState dans trictrac/store/src/game.rs.

L'application ligne de commande est implémentée dans 'trictrac/client_cli', elle permet de jouer contre un bot, ou de faire jouer deux bots l'un contre l'autre.
'trictrac/client_cli/src/game_runner.rs' contient la logique permettant de faire jouer deux bots l'un contre l'autre.
'trictrac/bot/src/strategy/default.rs' contient le code d'une stratégie de bot basique : il détermine la liste des mouvements valides (avec la méthode get_possible_moves_sequences de store::MoveRules) et joue simplement le premier de la liste.

## Plan d'action

Dans un premier temps je voudrais que dans le dossier open_spiel/open_spiel/games/trictrac tu fasse une adaptation en c++ des fichiers de ma version rust des règles du trictrac trictrac/store. On verra dans un second temps comment correctement utiliser cette bibliothèque au sein d'Open Spiel.

- suis les conventions C++ de Google <https://google.github.io/styleguide/cppguide.html>
- adapte les fichiers dans cet ordre de préférence : player.rs, dice.rs, board.rs, game_rules_moves.rs, game_rules_points.rs, game.rs
