#ifndef FONC_IA_H
#define FONC_IA_H

#define NEGA_INF -99999 ///< Heuristique minimum pour minmax
#define POS_INF 99999   ///< Heuristique maximum pour minmax

/**
* \file fonc_ia.h
* \brief Fonctions et structures de données pour l'I.A.
* \author Meriem Taieb Kherafa (heuristique), Chaosok Kong (minmax)
*
* Fonctions servant au fonctionnement de l'I.A.: algorithme minmax et heuristique. Il existe plusieurs modes de difficulté pour l'I.A.
*/

// Définition des types
typedef enum {UNK, FACILE, MOYEN, DIFFICILE} t_niveau;

// Définition des fonctions
int heuristique_facile(t_case jeu[N][N], t_case joueur);
int heuristique_avancee(t_case jeu[N][N], t_case joueur);
int minMax(t_case jeu[N][N], t_case etatOrig, t_case etatActuel, int profondeur, int max_profondeur, int (*heuristique)(t_case jeu[N][N], t_case joueur));
void minMaxChoix(t_case jeu[N][N], t_case joueur, int *x, int *y, t_niveau difficulte);

#endif
