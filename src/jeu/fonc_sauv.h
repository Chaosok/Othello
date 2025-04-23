#ifndef FONC_SAUV_H
#define FONC_SAUV_H
#define FICHIER_SAUVEGARDE "liste_sauvegarde.txt" ///< Nom du fichier de sauvegarde
#define MAX_SAUVEGARDE 5  ///< Nombre maximum de parties sauvegardées
#define MAX_NOM_FICHIER 50  ///< Taille maximum d'un nom de partie
#include "fonc_jeu.h"
#include "../IA/fonc_ia.h"


/**
* \file fonc_sauv.h
* \brief Fonctions pour la sauvegarde d'une partie othello.
* \author Chaosok Kong et Meriem Taieb Kherafa (supprimer_sauvegarde)
*/

//prototypage
void sauvegarderJeu(t_case jeu[N][N], t_case joueurActuel, t_niveau modeJeu, char nomFichier[50],  int temps_noir, int temps_blanc);
void sauvegarderJeu_bis(t_case jeu[N][N], t_case joueurActuel, char nomFichier[50]);
int chargerJeu(t_case jeu[N][N], t_case *joueurActuel, t_niveau *modeJeu, char nomFichier[50],  int *temps_noir, int *temps_blanc);
void afficher_liste();
void supprimer_sauvegarde(const char fichierASupprimer[MAX_NOM_FICHIER]);
#endif
