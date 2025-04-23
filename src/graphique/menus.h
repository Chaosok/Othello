#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "../jeu/fonc_sauv.h"
#include "../jeu/reseau.h"
#define MAX_IP_LENGTH 64

/**
* \file menus.c
* \brief Fonctions et structures de données pour l'affichage des menus.
* \author Nasreddine Biya, Alexandra Péan
*/
extern char ipInput[MAX_IP_LENGTH];
extern int textInputStarted;
extern char composition[MAX_IP_LENGTH];
extern Sint32 cursor;
extern Sint32 selection_len;

extern int serveurPret;
extern int clientAccepte;
extern int threadLance;

typedef enum {
    MENU_PLAY,
    MENU_MAIN,
    MENU_SUB,
    MENU_SETTINGS,
    MENU_RESEAU,
    MENU_CREER_PARTIE_RESEAU,
    MENU_REJOINDRE_PARTIE_RESEAU,
    MENU_LEVEL_IA,
    MENU_COUL_PIONS,
    MENU_SAUV_PARTIES
} MenuState;

typedef struct {
    SDL_Rect rect;
    SDL_Color normalColor;
    SDL_Color hoverColor;
    int isHovered;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
} Button;

typedef struct {
    int x, y, l, h;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
} bouton_t;

// Global menu variables
extern int gameMode;
extern Button boutonModeSombre;
extern Button boutonModeClair;
extern Button boutonRetour;
extern int themeMode; 

extern SDL_Texture* backgroundTexture;
extern SDL_Texture* settingsTexture;
extern SDL_Rect settingsRect;
extern Button buttons[3];
extern bouton_t bouton1, bouton2, bouton3, niveau1, niveau2, niveau3, couleur1, couleur2;
extern Button theme;
extern MenuState currentMenu;
extern int niveau_selectionne;
extern int volume;
extern SDL_Rect slidebar;
extern SDL_Rect slidehandler;
extern SDL_Rect slidebar2;
extern SDL_Rect slidehandler2;
extern int soundVolume;
extern Mix_Music *menuMusique;
extern int jouerMusique;
extern Mix_Chunk *sonPion;

typedef struct {
    char filename[MAX_NOM_FICHIER];
    Button button;
} SavedGame;

extern SavedGame savedGames[MAX_SAUVEGARDE];
extern int numSavedGames;

extern Button btnSupprimer;
extern int selection;
extern contexte_reseau_t ctx;


void initMainMenu(TTF_Font* menuFont, SDL_Renderer* renderer);
void initSubMenu(TTF_Font* menuFont, SDL_Renderer* renderer) ;
void initLevelMenu(TTF_Font* menuFont, SDL_Renderer* renderer);
void initCoulMenu(TTF_Font* menuFont, SDL_Renderer* renderer);
void initReseauMenu(TTF_Font* menuFont, SDL_Renderer* renderer);

void renderMainMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderSubMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderLevelMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderCoulMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderSettingsMenu(SDL_Renderer *renderer, TTF_Font *menuFont);
void renderButtonText(SDL_Renderer* renderer, Button* button) ;
void renderReseauMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderCreerPartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
void renderRejoindrePartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont, const char* ipInput);

void handleMainMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1);
void handleSubMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) ;
void handleLevelMenuEvents(SDL_Event *e, int *quit);
void handleCoulMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
void handleSettingsMenuEvents(SDL_Event *e, int *quit, SDL_Renderer *renderer, TTF_Font *menuFont);
void handleReseauMenuEvents(SDL_Event *e, int *quit) ;
void handleCreerPartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
void handleRejoindrePartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);


int checkButtonHover(Button* button, int mouseX, int mouseY) ;
void affiche_text(const char* txt, int x, int y, SDL_Color titleColor, SDL_Renderer* renderer, TTF_Font* font);

void initSauvMenu(TTF_Font* menuFont, SDL_Renderer* renderer) ;
void renderSauvMenu(SDL_Renderer* renderer, TTF_Font* menuFont) ;
void handleSauvMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);

#endif
