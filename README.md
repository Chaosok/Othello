# Othello

##### Date de début : 24/01/2025
##### Date de fin : 10/04/2025

## Description

Notre projet est une version numérique du jeu Othello, un jeu de stratégie conçu pour être joué à deux.

## Fonctionnalités

- [x] Détection automatique des coups valides
- [x] Deux joueurs humains peuvent s’affronter sur le même appareil ou en ligne
- [x] Un joueur humain peut jouer contre une IA (niveaux facile, moyen, difficile)
- [x] Une interface graphique qui permet de visualiser le plateau, les pions capturés, les coups possibles, les scores de chaque joueur...
- [x] Sauvegarder plusieurs parties

## Exécution

Dans le terminal, placez vous dans src/ et lancez :  
make && ./othello

## But du jeu

Le jeu Othello se joue sur un plateau de 64 cases appelé othellier. Chaque joueur dispose de 64 pions, l’un jouant avec les pions noirs et l’autre avec les pions blancs.Au début de la partie, quatre pions sont déjà placés au centre de l’othellier : deux noirs et deux blancs. Le joueur qui joue avec les pions noirs commence la partie.Les joueurs jouent ensuite à tour de rôle afin de capturer des pions adverses lors de leurs mouvements, sur une case libre du plateau.
Un joueur peut capturer les pions adverses si les conditions suivantes sont remplies :
- Le pion placé ferme un ou plusieurs alignements de pions adverses (horizontaux, verticaux ou diagonaux).
- Ces alignements sont encadrés à leurs deux extrémités par des pions de la couleur du joueur qui joue.

Tous les pions adverses situés dans ces alignements deviennent alors de la couleur du joueur qui les a capturés. Si le pion placé vient fermer plusieurs alignements, il capture tous les pions adverses concernés.
Le jeu se termine lorsque les deux joueurs ne peuvent plus poser de pion.Le joueur ayant le plus de pions de sa couleur sur l’othellier remporte la partie.

## Crédits

Ce projet a été développé par :
- Taieb Kherafa Meriem
- Biya Nasreddine
- Mahjoub Aly Rida
- Kong Chaosok
- Péan Alexandra


##### L2 Informatique, Université du Mans

