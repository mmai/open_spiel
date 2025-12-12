# Open Spiel Trictrac

## Installation

Autoriser direnv pour devenv automatique : `direnv allow`

Tester installation :

```sh
rm -rf build
just build
just play backgammon`
```

## TODO

- Etablir correspondances entre environnements d'entrainement burn-rl et openspiel
- créer nouveau jeu openspiel
- faire un wrapper ffi cc autour de rust pour utiliser les méthodes de l'environnement burn-rl
- si ça ne marche pas -> traduire les méthodes rust en c++. Partir de coquille vide et compiler étape par étape.
