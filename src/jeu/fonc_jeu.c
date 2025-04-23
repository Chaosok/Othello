#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fonc_jeu.h"
#include "fonc_sauv.h"

/**
* \file fonc_jeu.c
* \brief Définition des fonctions pour la logique de jeu.
* \author Aly Rida Mahjoub, Meriem Taieb Kherafa, Chaosok Kong
*/

/** 
* \fn init_jeu(t_case jeu[N][N])
* \brief Initialise le plateau de jeu.
* \param jeu Plateau de jeu.
*/
void init_jeu(t_case jeu[N][N]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            jeu[i][j] = vide;
    
    // Initialisation des quatre pions de départ au centre
    int mil = N / 2;
    jeu[mil - 1][mil - 1] = noir;
    jeu[mil][mil] = noir;
    jeu[mil - 1][mil] = blanc;
    jeu[mil][mil - 1] = blanc;
}


/** 
* \fn copyOthellier(t_case jeu_src[N][N], t_case jeu_copie[N][N])
* \brief Copie du plateau pour la simulation virtuelle.
* \param jeu_src Plateau source.
* \param jeu_copie Plateau destination.
*/
void copyOthellier(t_case jeu_src[N][N], t_case jeu_copie[N][N]){
    memcpy(jeu_copie, jeu_src, N*N*sizeof(t_case));
}

/** 
* \fn changerJoueur(t_case joueurActuel)
* \brief Alterne entre le joueur blanc et le joueur noir.
* \param joueurActuel Joueur actuel (noir ou blanc).
* \return Noir si le joueur actuel est blanc, blanc sinon.
*/
t_case changerJoueur(t_case joueurActuel){
    if(joueurActuel != vide)
        return (joueurActuel == noir ? blanc : noir);
    return vide; // cas invalide, aucune action
}

/** 
* \fn peut_jouer(t_case jeu[N][N], t_case joueur)
* \brief  Vérifie si le joueur peut jouer au moins un coup valide.
* \param jeu Plateau de jeu.
* \param joueur Joueur actuel.
* \return 1 si le joueur peut jouer, 0 sinon.
*/
int peut_jouer(t_case jeu[N][N], t_case joueur) {
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            if (coup_valide(jeu, x, y, joueur)) {
                return 1;  // Un coup est possible, le joueur peut jouer
            }
        }
    }
    return 0;  // Aucun coup valide disponible
}

/** 
* \fn partie_terminee(t_case jeu[N][N])
* \brief Vérifie si la partie est finie.
* \param jeu Plateau de jeu.
* \return 1 si la partie est finie, 0 sinon.
*/
int partie_terminee(t_case jeu[N][N]) {
    return (!peut_jouer(jeu, noir) && !peut_jouer(jeu, blanc));
}

/** 
* \fn afficher_jeu(t_case jeu[N][N])
* \brief Affiche l'état actuel du plateau de jeu dans la console.
* \param jeu Plateau de jeu.
*/
void afficher_jeu(t_case jeu[N][N]) {
    // Affichage des numéros de colonnes (de 1 à N)
    printf("  ");
    for(int i = 0; i < N; i++)
        printf(" %d ", i + 1);
    printf("\n");

    // Affichage des lignes du plateau avec les pions
    for(int i = 0; i < N; i++){        
        printf("%d ", i + 1);  // Affiche le numéro de la ligne
        for(int j = 0; j < N; j++)
            switch(jeu[i][j]){ 
                case vide: printf("[ ]"); break;
                case blanc: printf("[B]"); break;
                case noir: printf("[N]"); break;
            }
        printf("\n");
    }
}

/** 
* \fn coup_valide(t_case jeu[N][N], int x, int y, t_case couleur)
* \brief Vérifie si un coup est valide à une position donnée (x, y) pour une couleur donnée.
* \param jeu Plateau de jeu.
* \param x Coordonnée x de la case à vérifier.
* \param y Coordonnée y de la case à vérifier.
* \param couleur: Couleur du pion à poser.
* \return 0 si le coup n'est pas valide, valeur >0 sinon.
*/
int coup_valide(t_case jeu[N][N], int x, int y, t_case couleur) {
    if (jeu[x][y] != vide) return 0;

    t_case simul[N][N];
    copyOthellier(jeu, simul);
    return retourner_pions(simul, x, y, couleur);
}

/** 
* \fn retourner_pions(t_case jeu[N][N], int x, int y, t_case couleur)
* \brief Retourne les pions adverses autour de la case (x, y) après un coup valide.
* \param jeu Plateau de jeu.
* \param x Coordonnée x de la case où poser le pion.
* \param y Coordonnée y de la case où poser le pion.
* \param couleur: Couleur du pion à poser.
* \return Nombre de pions adverses retournés.
*/
int retourner_pions(t_case jeu[N][N], int x, int y, t_case couleur) {
    // Définition des directions pour retourner les pions (8 directions possibles)
    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };
    
    // Compteur de pions retournés
    int compteur = 0;

    // Vérification des directions autour de la case (x, y)
    for (int d = 0; d < 8; d++) {
        int dx = directions[d][0], dy = directions[d][1];
        int i = x + dx, j = y + dy, count = 0;

        // Recherche d'une séquence de pions adverses
        while (i >= 0 && i < N && j >= 0 && j < N && jeu[i][j] != vide && jeu[i][j] != couleur) {
            i += dx; j += dy; count++;
        }

        // Si une séquence est trouvée suivie d'un pion de la couleur du joueur, retourner les pions
        if (count > 0 && i >= 0 && i < N && j >= 0 && j < N && jeu[i][j] == couleur) {
            for (int k = 1; k <= count; k++) {
                jeu[x + k * dx][y + k * dy] = couleur;  // Retourne les pions adverses
                compteur++;
            }
        }
    }

    return compteur;
}

/** 
* \fn score(t_case jeu[N][N], t_case couleur)
* \brief Calcule le score du joueur actuel.
* \param jeu Plateau de jeu.
* \param couleur Couleur du joueur actuel.
* \return Nombre de pions du joueur sur le plateau.
*/
int score(t_case jeu[N][N], t_case couleur){
    int nb_pions = 0;
    for(int i = 0; i < N; i++){        
        for(int j = 0; j < N; j++){
            if(jeu[i][j] == couleur){
                nb_pions++;
            }
        }
    }
    return nb_pions;
}


   
   