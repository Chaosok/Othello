#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jeu/fonc_jeu.h"
#include "fonc_ia.h"

/**
* \file fonc_ia.c
* \brief Définition des fonctions pour l'I.A.
* \author Meriem Taieb Kherafa (heuristique), Chaosok Kong (minmax)
*/

// FONCTIONS D'ÉVALUATION


/** 
* \fn eval_score(t_case jeu[N][N], t_case joueur)
* \brief Sert à évaluer la différence des scores.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière (positive: favorable au joueur, négative: favorable à l'adversaire).
*/
int eval_score(t_case jeu[N][N], t_case joueur){
    t_case adversaire = changerJoueur(joueur);
    return (score(jeu, joueur) - score(jeu, adversaire)) * 2;
}


/** 
* \fn eval_coins(t_case jeu[N][N], t_case joueur)
* \brief Sert à évaluer si les positions stratégiques (les coins) sont prises ou libres.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière (positive: favorable au joueur, négative: favorable à l'adversaire).
*/
int eval_coins(t_case jeu[N][N], t_case joueur){
    int nb_coins = 0, adversaire = changerJoueur(joueur);
    int pos_coins[4][2] = {{0, 0}, {0, N - 1}, {N - 1, 0}, {N - 1, N - 1}};
    t_case coin;

    for(int i = 0; i < 4; i++){
        coin = jeu[pos_coins[i][0]][pos_coins[i][1]];
        
        if(coin == joueur)
            nb_coins++;
        else if((int)coin == adversaire)
            nb_coins--;
    }

    return nb_coins * 8;
}

/** 
* \fn eval_mobilite(t_case jeu[N][N], t_case joueur)
* \brief Sert à évaluer la mobilité du joueur (le nombre de coups qu'il peut jouer). La mobilité est plus importante en début de partie.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière (positive: favorable au joueur, négative: favorable à l'adversaire).
*/
int eval_mobilite(t_case jeu[N][N], t_case joueur){
    int mobilite = 0;
    int nb_coups_restants = score(jeu, vide);

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            // on compte toutes les cases vides et on déduit les cases jouables par l'adversaire
            if(jeu[i][j] == vide)
                mobilite++;
            if(coup_valide(jeu, i, j, changerJoueur(joueur)))
                mobilite--;
            }
    
    return mobilite * (nb_coups_restants / 2);
}

/** 
* \fn eval_parite(t_case jeu[N][N], t_case joueur)
* \brief Sert à évaluer qui jouera le dernier coup.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière.
*/
int eval_parite(t_case jeu[N][N], t_case joueur){
    int nb_coups_joues = score(jeu, joueur) + score(jeu, changerJoueur(joueur));
    return (nb_coups_joues % 2) * 2;  // poser le dernier pion présente un avantage
}

/** 
* \fn heuristique(t_case jeu[N][N], t_case joueur, int (*fonctions[])(t_case jeu[N][N], t_case joueur), int nb_fonc)
* \brief Fonction d'heuristique générique. Modulable.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \param fonctions Liste de pointeurs sur des fonctions d'évaluation.
* \return Valeur entière représentant l'heuristique.
*/
int heuristique(t_case jeu[N][N], t_case joueur, int (*fonctions[])(t_case jeu[N][N], t_case joueur), int nb_fonc){
    int eval = 0;

    for(int i = 0; i < nb_fonc; i++)
        eval += fonctions[i](jeu, joueur);
    
    return eval; 
}

/** 
* \fn heuristique_facile(t_case jeu[N][N], t_case joueur)
* \brief Version "facile" de l'I.A.: appelle une fonction calculant le score seulement.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière représentant l'heuristique.
*/
int heuristique_facile(t_case jeu[N][N], t_case joueur){ 
    int (*fonc[])(t_case jeu[N][N], t_case joueur) = {eval_score};
    return(heuristique(jeu, joueur, fonc, 1));
}

/** 
* \fn heuristique_avancee(t_case jeu[N][N], t_case joueur)
* \brief Version "avancée" de l'I.A.: appelle des fonctions calculant le score, la mobilité, les positions stratégiques et la parité.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \return Valeur entière représentant l'heuristique.
*/
int heuristique_avancee(t_case jeu[N][N], t_case joueur){ 
    int (*fonc[])(t_case jeu[N][N], t_case joueur) = {eval_score, eval_coins, eval_mobilite, eval_parite};
    return(heuristique(jeu, joueur, fonc, 4));
}

/** 
* \fn minMax(t_case jeu[N][N], t_case etatOrig, t_case etatActuel, int profondeur, int max_profondeur, int (*heuristique)(t_case jeu[N][N], t_case joueur))
* \brief Algorithme minmax qui calcule l'arbre des coups possibles.
* \param jeu Plateau de jeu de taille N x N.
* \param etatOrig Couleur du joueur original (celui pour lequel on calcule l'heuristique).
* \param etatActuel Couleur du joueur actuel (celui du niveau de l'arbre sur lequel on se trouve).
* \param profondeur Profondeur actuelle de l'arbre.
* \param max_profondeur Profondeur maximum.
* \param heuristique Fonction calculant l'heuristique.
* \return Valeur entière représentant la meilleure heuristique trouvée.
*/
int minMax(t_case jeu[N][N], t_case etatOrig, t_case etatActuel, int profondeur, int max_profondeur, int (*heuristique)(t_case jeu[N][N], t_case joueur)){
    //condition d'arret
    if (profondeur == max_profondeur || partie_terminee(jeu))
    return heuristique(jeu, etatOrig);
   
    int meilleurVal = (etatOrig == etatActuel) ? NEGA_INF : POS_INF;
   
    //si c'est au joueur initial de jouer, on cherche la valeur MAX
    //sinon MIN
   
    //parcours des cases de l'othellier
    //1.copie le plateau
    //2.placer un pions à i,j
    //3.simuler les coups possibles de l'adversaire avec retourner_pions
   
    for (int i = 0; i<N; i++){
   
        for (int j = 0; j<N; j++){
    
            if (coup_valide(jeu, i, j, etatActuel)){
                t_case othellierTempo[N][N];
                copyOthellier(jeu, othellierTempo);
                othellierTempo[i][j] = etatActuel;
                retourner_pions(othellierTempo, i, j, etatActuel);

                //appel recursif, pour evaluer le jeu et changer le tour du joueur
                int val = minMax(othellierTempo, etatOrig, (etatActuel == noir)?blanc: noir, profondeur+1, max_profondeur, heuristique);
            
                // mise a jour de la meilleur valeur
                if (etatOrig == etatActuel){          
                    if (val > meilleurVal)
                        meilleurVal = val;
                }
                else
                    if (val < meilleurVal)
                        meilleurVal = val;
            }
        }
    }
    return meilleurVal;
}

/** 
* \fn minMaxChoix(t_case jeu[N][N], t_case joueur, int *x, int *y, t_niveau difficulte)
* \brief Choix du meilleur coup avec minmax.
* \param jeu Plateau de jeu de taille N x N.
* \param joueur Couleur du joueur actuel.
* \param x Coordonnée x du coup choisi (sortie).
* \param y Coordonnée y du coup choisi (sortie).
* \param difficulte Niveau de difficulté de l'I.A.
* \return Valeur entière représentant l'heuristique.
*/
extern void minMaxChoix(t_case jeu[N][N], t_case joueur, int *x, int *y, t_niveau difficulte){
   
    int meilleurVal = NEGA_INF;
    int meilleur_x = -1, meilleur_y = -1;
    int max_profondeur;
    int (*heuristique)(t_case jeu[N][N], t_case joueur);

    switch(difficulte){
        case FACILE:
            max_profondeur = 1;
            heuristique = heuristique_facile;
            break;
        case MOYEN:
            max_profondeur = 2;
            heuristique = heuristique_facile;
            break;
        case DIFFICILE:
            max_profondeur = 4;
            heuristique = heuristique_avancee;
            break;
        default:
            break;
    }
      
    for (int i = 0; i<N; i++){
        for (int j = 0; j<N; j++){
            if (coup_valide(jeu, i, j, joueur)){
                t_case othellierTempo[N][N];

                copyOthellier(jeu, othellierTempo);
                othellierTempo[i][j] = joueur;
                retourner_pions(othellierTempo, i, j, joueur);
                int val = minMax(othellierTempo, joueur, (joueur == noir)?blanc: noir, 1, max_profondeur, heuristique);

                //mise a jour du meilleur coup
                if (val > meilleurVal){
                    meilleurVal = val;
                    meilleur_x = i;
                    meilleur_y = j;
                }
            }
        }
    }
   
    //mettre a jour les coordonées
    *x = meilleur_x;
    *y = meilleur_y;
}
