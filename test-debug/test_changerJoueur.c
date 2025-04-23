#include <stdio.h>
#include <assert.h>
#include "../src/jeu/fonc_jeu.h"

/*
    Test de changerJoueur (src/jeu/fonc_jeu.c)
*/

/**
* \file test_changerJoueur.c
* \brief Tests unitaires sur la fonction changerJoueur
* \author Meriem Taieb Kherafa
*/


int main(){
    t_case valeur = blanc;

    assert (changerJoueur(valeur) == noir);

    valeur = noir;
    assert (changerJoueur(valeur) == blanc);

    valeur = vide;
    assert (changerJoueur(valeur) == vide);

    printf("Les tests sont passés \n");

    return 0;
}
