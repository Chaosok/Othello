#ifndef PLATEAU_H
#define PLATEAU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> 
#include "../jeu/fonc_jeu.h"
#include "../jeu/fonc_sauv.h"
#include "../IA/fonc_ia.h"
#include "menus.h"

/**
* \file plateau.h
* \brief Fonctions et structures de données pour l'affichage du plateau de jeu. 
* \author Alexandra Péan
*/

#define TEMPS_LIMITE 150 ///< Chronomètre pour chaque joueur
#define LARGEUR_MIN_FENETRE 1040 ///< Largeur minimum de la fenêtre de jeu
#define HAUTEUR_MIN_FENETRE 580 ///< Hauteur minimum de la fenêtre de jeu
#define HAUTEUR_DEBLOCAGE 780 ///< Seuil pour débloquer la largeur
#define LARGEUR_MAX_FENETRE 1500 ///< Largeur maximum de la fenêtre de jeu

extern SDL_Window* window;
extern Mix_Chunk *sonPion;


// Structure qui regroupe les dimensions et positions
typedef struct {
    int largeurFenetre;
    int hauteurFenetre;
    int tailleCase;
    int taillePolice;
    int largeurPlateau;
    int hauteurPlateau;
    int posXPlateau;
    int posYPlateau;
    int rayonPionScoreJ;
    int largeurBouton;
    int hauteurBouton;
    int boutonX;
    int boutonY;
} DimensionsJeu;

// Structure pour un bouton
typedef struct {
    SDL_Rect rect; // zone bouton
    SDL_Color color;  // couleur du bouton
    char text[20];  // texte dans le bouton
} bouton;

extern t_case coul; ///< Couleur des pions du joueur

/* Fonctions d'affichage */
void dessinerPion(SDL_Renderer *renderer, int centreX, int centreY, int rayon, SDL_Color couleur);
void afficherPlateau(SDL_Renderer *renderer, t_case jeu[N][N], int tailleCase, int posX, int posY);
void afficherScore(SDL_Renderer* renderer, TTF_Font* font1,TTF_Font* font2, 
    int scoreNoir, int scoreBlanc, int hauteurFenetre, int largeurFenetre, int rayonPion);
void affichageTourJoueur(SDL_Renderer* renderer, TTF_Font* font, int largeurFenetre, int posYPlateau, t_case joueurActuel);
void afficher_coups_possibles(SDL_Renderer *renderer, t_case jeu[N][N], t_case couleur, int tailleCase, int posX, int posY);
void afficher_bouton(SDL_Renderer *renderer, TTF_Font *font, bouton *btn);
int afficherFenetreFinDePartie(SDL_Renderer* renderer, int scoreNoir, int scoreBlanc, 
    int largeurFenetre, int hauteurFenetre, int *temps_noir, int *temps_blanc, t_case joueurActuel);
void afficherChrono(SDL_Renderer *renderer, TTF_Font *font, int temps_rest, int x, int y, 
    int posImgX, int posImgY, int largeurFenetre);
void creerMessageSauvegarde(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, 
        SDL_Texture** texture, SDL_Rect* rect, int largeurFenetre, int hauteurFenetre);

/* Fonctions de gestion des clics de l'utilisateur*/
void gererClics(SDL_Event *e, t_case jeu[N][N], t_case *joueurActuel, int tailleCase, 
    int positionPlateauX, int positionPlateauY, 
    int *temps_noir, int *temps_blanc, time_t *dernier_tour);
int clicBouton(bouton *btn, int mouseX, int mouseY);

/* Fonctions de gestion des dimensions de chaque objet */
void initialiserDimensions(DimensionsJeu* dims);
void mettreAJourDimensions(DimensionsJeu* dims, bouton* btnSauvegarde, TTF_Font** font1, TTF_Font** font2);

/* Fonctions de gestion des ressources */
int initialiserSDL(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font1, TTF_Font** font2, DimensionsJeu* dims);
void nettoyerRessources(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);

/* Fonctions de jeu */
int jouerPartie(int modeOrdi, int partieChargee, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char * fichier);
int JoueurVsOrdi(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul);
int JoueurVsJoueur(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
int JoueurVsOrdiSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char * fichier);
int JoueurVsJoueurSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, char * fichier);
int afficherFenetreFinDePartie_bis(SDL_Renderer* renderer, int scoreNoir, int scoreBlanc, int largeurFenetre, int hauteurFenetre, int *temps_noir, int *temps_blanc, t_case joueurActuel);
#endif