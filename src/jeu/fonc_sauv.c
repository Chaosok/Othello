#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fonc_jeu.h"
#include "fonc_sauv.h"
#include "../IA/fonc_ia.h"

/**
* \file fonc_sauv.c
* \brief Définition des fonctions pour la sauvegarde.
* \author Chaosok Kong, Meriem Taieb Kherafa (supprimer_partie)
*/

/** 
* \fn sauvegarderJeu(t_case jeu[N][N], t_case joueurActuel, char nomFichier[50])
* \brief Sauvegarder le jeu dans un fichier texte.
* \param jeu Plateau de jeu.
* \param joueurActuel Joueur actuel.
* \param modeJeu Mode de jeu (humain/difficulté de l'IA).
* \param nomFichier Nom du fichier de sauvegarde.
*/
void sauvegarderJeu(t_case jeu[N][N], t_case joueurActuel, t_niveau modeJeu, char nomFichier[50], int temps_noir, int temps_blanc) {
    FILE *fichier = fopen(nomFichier, "w");
    if (fichier == NULL) {
        printf("Erreur lors de la sauvegarde.\n");
        return;
    }

    // Sauvegarder le joueur actuel
    fprintf(fichier, "%d\n", joueurActuel);

    // Sauvegarder le mode de jeu
    fprintf(fichier, "%d\n", modeJeu);
    
    // Sauvegarder les chronos
    fprintf(fichier, "%d\n", temps_noir);
    fprintf(fichier, "%d\n", temps_blanc);

    // Sauvegarder othellier
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fichier, "%d ", jeu[i][j]);
        }
        fprintf(fichier, "\n");
    }
    
    fclose(fichier);
    printf("Partie sauvegardée avec succès.\n");

    //fichier liste de nom sauvegarde
    FILE * f = fopen(FICHIER_SAUVEGARDE, "a");
    if (fichier == NULL)
        return;
    fprintf(f, "%s\n", nomFichier);
    fclose(f);
}

/** 
* \fn chargerJeu(t_case jeu[N][N], t_case *joueurActuel, char nomFichier[50])
* \brief Charger une partie depuis un fichier texte.
* \param jeu Plateau de jeu.
* \param joueurActuel Joueur actuel.
* \param modeJeu Mode de jeu (humain/difficulté de l'IA).
* \param nomFichier Nom du fichier de sauvegarde.
* \return 1 si le chargement s'est effectué sans erreur, 0 sinon (fichier introuvable).
*/
int chargerJeu(t_case jeu[N][N], t_case *joueurActuel, t_niveau *modeJeu, char nomFichier[50], int *temps_noir, int *temps_blanc) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Aucune sauvegarde trouvée.\n");
        return 0;
    }

    // Charger le joueur actuel
    int tempJoueur;
    fscanf(fichier, "%d", &tempJoueur);
    *joueurActuel = (t_case)tempJoueur;

    // Charger le mode de jeu
    int tempMode;
    fscanf(fichier, "%d", &tempMode);
    *modeJeu = (t_niveau)tempMode;

    // Charger les chronos
    int tempTempsnoir, tempTempsblanc;
    fscanf(fichier, "%d", &tempTempsnoir);
    fscanf(fichier, "%d", &tempTempsblanc);
    (*temps_noir) = tempTempsnoir;
    (*temps_blanc) = tempTempsblanc;

    // Charger othellier
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int tempCase;
            fscanf(fichier, "%d", &tempCase);
            jeu[i][j] = (t_case)tempCase;
        }
    }

    fclose(fichier);
    return 1;
}

/** 
* \fn afficher_liste()
* \brief Affiche la liste des sauvegardes disponibles.
*/
void afficher_liste(){
    char nomFichier[50];
    FILE * fichier = fopen("liste_sauvegarde.txt", "r");
    if (fichier ==NULL)
        return;
    printf("Liste des sauvegardes disponibles :\n");
    while (fscanf(fichier, "%s", nomFichier) == 1) {
        printf("- %s\n", nomFichier);
    }
    
    fclose(fichier);
}

/**
* \fn mystrdup(const char * src)
* \brief Redéfition de strdup.
* \param src nom du fichier source.
*/
char * mystrdup(const char * src) {
    char * dup = malloc(strlen(src) + 1);
    if (dup != NULL)
        strcpy(dup, src);
    return dup;
}

/**
* \fn supprimer_sauvegarde (const char fichierASupprimer[MAX_NOM_FICHIER])
* \brief Supprimer une sauvegarde dont le nom est passé en paramètre.
* \param fichierASupprimer Nom du fichier de sauvegarde à supprimer.
*/
void supprimer_sauvegarde(const char fichierASupprimer[MAX_NOM_FICHIER]) {
    FILE *fichier = fopen(FICHIER_SAUVEGARDE, "r");
    if (!fichier) {
        printf("Fichier sauvegarde introuvable\n");
        return;
    }

    char *liste_noms[MAX_SAUVEGARDE];
    char ligne[MAX_NOM_FICHIER + 2];
    int i = 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL && i < MAX_SAUVEGARDE) 
        liste_noms[i++] = mystrdup(ligne);
    
    fclose(fichier);

    fichier = fopen(FICHIER_SAUVEGARDE, "w");
    if (!fichier) {
        printf("Fichier sauvegarde introuvable\n");
        return;
    }

    size_t len = strlen(fichierASupprimer);
    for (int j = 0; j < i; j++) {
        if (strncmp(liste_noms[j], fichierASupprimer, len) != 0 || liste_noms[j][len] != '\n') {
            fputs(liste_noms[j], fichier);
        }
        free(liste_noms[j]);
    }

    fclose(fichier);
    remove(fichierASupprimer);
}