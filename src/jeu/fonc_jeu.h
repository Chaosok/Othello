#ifndef FONC_JEU_H
#define FONC_JEU_H
#define N 8 ///< Taille du plateau

/**
* \file fonc_jeu.h
* \brief Fonctions et structures de données nécessaires à la logique de jeu.
* \author Aly Rida Mahjoub, Meriem Taieb Kherafa, Chaosok Kong
*/

// Définition des types
typedef enum {blanc, noir, vide} t_case;

// Prototypage
void init_jeu(t_case jeu[N][N]);
int peut_jouer(t_case jeu[N][N], t_case joueur);
int partie_terminee(t_case jeu[N][N]);
void afficher_jeu(t_case jeu[N][N]);
int coup_valide(t_case jeu[N][N], int x, int y, t_case couleur);
int retourner_pions(t_case jeu[N][N], int x, int y, t_case couleur);
void copyOthellier(t_case jeu_src[N][N], t_case jeu_copie[N][N]);
t_case changerJoueur(t_case joueurActuel);
int score(t_case jeu[N][N], t_case couleur);

#endif
