# TODO

## But du projet

Le dossier forks/open_spiel contient mon fork du projet Open Spiel de Google DeepMind. J'ai créé un nouveau dossier open_spiel/games/trictrac en copiant les fichiers de open_spiel/games/backgammon. Je veux donc maintenant implémenter le jeu de Trictrac pour pouvoir entrainer un agent avec les algorithmes disponibles dans Open Spiel. Le principe est d'adapter les classes TrictracGame et TrictracState pour respecter les règles du Trictrac.

J'ai déjà fait une implémentation du jeu de Trictrac en rust dans le dossier ./trictrac/ et je voudrais m'en inspirer pour faire la version c++ pouvant être intégrée dans Open Spiel.

## Structure du projet rust

Les règles du jeu et l'état d'une partie sont implémentées dans 'trictrac/store'.
Plus précisément, l'état du jeu est défini par le struct GameState dans trictrac/store/src/game.rs.

L'application ligne de commande est implémentée dans 'trictrac/client_cli', elle permet de jouer contre un bot, ou de faire jouer deux bots l'un contre l'autre.
'trictrac/client_cli/src/game_runner.rs' contient la logique permettant de faire jouer deux bots l'un contre l'autre.
'trictrac/bot/src/strategy/default.rs' contient le code d'une stratégie de bot basique : il détermine la liste des mouvements valides (avec la méthode get_possible_moves_sequences de store::MoveRules) et joue simplement le premier de la liste.

## Plan d'action

Je voudrais que dans le dossier open_spiel/games/trictrac tu fasse une adaptation en c++ des structures de ma version rust du trictrac nécessaires pour répondre aux prérequis Open Spiel.

Peux-tu lister les étapes pour réaliser ce projet ?

---

Absolument. Voici une proposition de plan d'action détaillé pour intégrer le jeu de Trictrac dans OpenSpiel
en se basant sur votre implémentation Rust.

Ce plan est décomposé en étapes logiques, de l'analyse à la validation finale.

Plan d'Action : Implémentation du Trictrac dans Open Spiel

Étape 1 : Analyse de l'existant (Rust et C++)

- Objectif : Comprendre précisément les structures de données en Rust et l'API requise par Open Spiel.
- Actions :
  1.  Analyser `GameState` en Rust : Examiner en détail le fichier
      /home/henri/travaux/programmes/trictrac/store/src/game.rs pour lister tous les champs qui
      définissent l'état du jeu :
      - Représentation du plateau (les 24 flèches, le nombre de dames par flèche).
      - Dames sorties et dames sur la barre.
      - État des dés (valeurs, dés utilisés).
      - Joueur courant.
      - Scores des joueurs.
      - Phase du jeu (début, jeu normal, fin de partie).
  2.  Analyser la génération de coups en Rust : Étudier la logique de la fonction
      get_possible_moves_sequences pour comprendre comment les coups valides sont générés à partir d'un
      état et d'un lancer de dés.
  3.  Analyser l'exemple Backgammon en C++ : Étudier les fichiers open_spiel/games/backgammon.h et
      open_spiel/games/backgammon.cc pour identifier les classes et méthodes clés à implémenter :
      - La classe TrictracGame (dérivée de open_spiel::Game) : responsable de fournir les informations
        générales sur le jeu (nombre de joueurs, etc.) et de créer un état initial.
      - La classe TrictracState (dérivée de open_spiel::State) : représente l'état d'une partie, et
        contient la logique principale (ApplyAction, LegalActions, IsTerminal, etc.).

Étape 2 : Mise en place du squelette C++

- Objectif : Avoir une base de code C++ compilable en renommant les fichiers et classes du Backgammon.
- Actions :
  1.  Renommer les fichiers backgammon.h et backgammon.cc en trictrac.h et trictrac.cc.
  2.  Dans ces fichiers, remplacer toutes les occurrences de Backgammon par Trictrac et backgammon par
      trictrac.
  3.  Mettre à jour le fichier open_spiel/games/trictrac/CMakeLists.txt pour qu'il référence les nouveaux
      noms de fichiers.

Étape 3 : Adaptation de la structure `GameState` (Rust) vers la classe `TrictracState` (C++)

- Objectif : Traduire la représentation de l'état du jeu de Rust en C++.
- Actions :
  1.  Dans trictrac.h, modifier les membres privés de la classe TrictracState pour qu'ils correspondent
      aux champs de la structure GameState Rust. Par exemple :
      - std::array<int, 26> board\_: Un tableau pour les 24 flèches, plus la barre (point 0) et les dames
        sorties (point 25).
      - int current*player*: Le joueur dont c'est le tour.
      - std::vector<int> dice\_: Les valeurs des dés pour le tour en cours.
      - std::array<int, 2> scores\_: Les scores des deux joueurs.

Étape 4 : Implémentation de la logique du jeu dans `TrictracState`

- Objectif : Porter la logique des règles du jeu de Rust vers les méthodes requises par l'API Open Spiel.
- Actions :
  1.  `TrictracState::NewInitialState()` : Implémenter la fonction qui initialise le plateau de jeu dans sa
      configuration de départ.
  2.  `TrictracState::LegalActions()` : C'est l'étape la plus complexe. Il faudra ici traduire la logique
      de get_possible_moves_sequences de Rust pour générer tous les coups possibles pour le joueur courant
      en fonction des dés. Chaque coup (ou séquence de mouvements) sera représenté par un Action.
  3.  `TrictracState::ApplyAction()` : Implémenter la logique qui modifie l'état du jeu (board*,
      current_player*, etc.) en fonction de l'action choisie par un joueur.
  4.  `TrictracState::IsTerminal()` : Écrire la fonction qui vérifie si la partie est terminée (par
      exemple, si un joueur a sorti toutes ses dames).
  5.  `TrictracState::Returns()` et `TrictracState::PlayerReturn()` : Définir les gains pour chaque joueur
      à la fin de la partie (+1 pour le gagnant, -1 for le perdant).
  6.  `TrictracState::ToString()` : Créer une représentation textuelle du plateau pour le débogage.

Étape 5 : Configuration de la classe `TrictracGame`

- Objectif : Fournir les méta-informations sur le jeu de Trictrac au framework Open Spiel.
- Actions :
  1.  Dans trictrac.cc, ajuster les paramètres du jeu dans le constructeur de TrictracGame :
      - NumPlayers() -> 2
      - MinUtility() -> -1.0
      - MaxUtility() -> 1.0
      - MaxGameLength() -> Une estimation raisonnable du nombre maximum de coups.

Étape 6 : Intégration dans le système de build d'Open Spiel

- Objectif : Rendre le nouveau jeu visible et compilable par le projet principal.
- Actions :
  1.  Ajouter add_subdirectory(games/trictrac) dans le fichier open_spiel/CMakeLists.txt principal.
  2.  Lancer une première compilation pour identifier et corriger les erreurs de syntaxe et d'intégration.

Étape 7 : Écriture d'un test unitaire

- Objectif : Valider le bon fonctionnement de l'implémentation.
- Actions :
  1.  Créer un fichier trictrac_test.cc en s'inspirant de backgammon_test.cc.
  2.  Écrire un test simple qui :
      - Crée un nouveau jeu.
      - Vérifie que l'état initial est correct.
      - Récupère les actions légales.
      - Applique une de ces actions.
      - Vérifie que le nouvel état du jeu est celui attendu.
  3.  Compiler et lancer les tests.

Une fois ces étapes terminées, le jeu de Trictrac sera fonctionnel au sein d'Open Spiel, et vous pourrez
commencer à l'utiliser avec les algorithmes d'apprentissage par renforcement fournis.

Souhaitez-vous que je commence par la première étape, l'analyse des fichiers ?
