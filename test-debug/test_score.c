#include <stdio.h>
#include <assert.h>
#include "../src/jeu/fonc_jeu.h"

/*
    Test de score (src/jeu/fonc_jeu.c)
*/


/**
* \file test_score.c
* \brief Tests unitaires sur la fonction score
* \author Meriem Taieb Kherafa
*/


// Remplir un plateau d'une couleur
void remplir(t_case jeu[N][N], t_case coul){
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            jeu[i][j] = coul;
}

// Remplir les bords d'un plateau d'une couleur seulement
void remplir_bords(t_case jeu[N][N], t_case coul){
    remplir(jeu, vide);

    int i;

    for(i = 0; i < N; i++)
        jeu[i][0] = coul;

    for(i = 1; i < N; i++)
        jeu[0][i] = coul;

    for(i = 1; i < N; i++)
        jeu[i][N - 1] = coul;

    for(i = 1; i < N - 1; i++)
        jeu[N - 1][i] = coul;
}

int main(){
    t_case jeu[N][N];
    int nb_cases = N * N;
    int nb_bords = nb_cases - ((N - 2) * (N - 2));

    // Jeu plein de pions blancs
    remplir(jeu, blanc);
    assert(score(jeu, blanc) == nb_cases);
    assert(score(jeu, noir) == 0);
    assert(score(jeu, vide) == 0);
    
    // Jeu plein de pions noirs
    remplir(jeu, noir);
    assert(score(jeu, noir) == nb_cases);
    assert(score(jeu, blanc) == 0);
    assert(score(jeu, vide) == 0);
    
    // Jeu vide
    remplir(jeu, vide);
    assert(score(jeu, noir) == 0);
    assert(score(jeu, blanc) == 0);
    assert(score(jeu, vide) == nb_cases);

    // Jeu avec bords blancs
    remplir_bords(jeu, blanc);
    assert(score(jeu, blanc) == nb_bords);
    assert(score(jeu, noir) == 0);
    assert(score(jeu, vide) == nb_cases - nb_bords);
    
    // Jeu avec bords noirs
    remplir_bords(jeu, noir);
    assert(score(jeu, noir) == nb_bords);
    assert(score(jeu, blanc) == 0);
    assert(score(jeu, vide) == nb_cases - nb_bords);

    printf("Les tests sont passés \n");
    return 0;
}
