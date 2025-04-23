#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include "menus.h"
#include "plateau.h"
#include <string.h>
#include "../jeu/reseau.h"


/**
* \file menus.c
* \brief Définition des fonctions pour l'affichage des menus.
* \author Nasreddine Biya, Alexandra Péan (menu choix de couleur des pions, menu liste de sauvegarde)
*/

char ipInput[MAX_IP_LENGTH] = "";
int textInputStarted = 0;
char composition[MAX_IP_LENGTH] = "";
Sint32 cursor = 0;
Sint32 selection_len = 0;

int gameMode = 0;
t_niveau difficultyLevel = FACILE;
int coulPion = 0;
int themeMode = 0; // 0 pour mode clair, 1 pour mode sombre
contexte_reseau_t ctx;
int serveurPret = 0;
int clientAccepte = 0;
int threadLance = 0;

Button boutonModeSombre;
Button boutonModeClair;
Button boutonRetour;

SDL_Texture* backgroundTexture = NULL;
SDL_Texture* settingsTexture = NULL;
SDL_Rect settingsRect;
Button buttons[3];
bouton_t bouton1, bouton2, bouton3, bouton4, bouton5, niveau1, niveau2, niveau3, couleur1, couleur2;
Button theme;
MenuState currentMenu = MENU_MAIN;
int niveau_selectionne = 0;
int volume = 50;
SDL_Rect slidebar = {260,200,200,10};
SDL_Rect slidehandler = {260,195,10,20};
SDL_Rect slidebar2 = {260,260,200,10};
SDL_Rect slidehandler2 = {260,255,10,20};

DimensionsJeu dims;

int numSavedGames = 0;
SavedGame savedGames[MAX_SAUVEGARDE] = {0};

Button btnSupprimer;
int selection;
int soundVolume = 120;

Mix_Music *menuMusique = NULL;
int jouerMusique = 0;
Mix_Chunk *sonPion = NULL;

/**
 * @brief Affiche le texte d’un bouton sur le rendu SDL.
 * @param renderer Le renderer SDL utilisé pour dessiner.
 * @param button Le pointeur vers le bouton contenant la texture du texte.
 */
void renderButtonText(SDL_Renderer* renderer, Button* button) {
    if (button->textTexture != NULL)
        SDL_RenderCopy(renderer, button->textTexture, NULL, &button->textRect);
}

/**
 * @brief Vérifie si la souris survole un bouton.
 *
 * Détermine si les coordonnées de la souris sont à l’intérieur
 * des limites du rectangle du bouton.
 *
 * @param button Pointeur vers le bouton à tester.
 * @param mouseX Coordonnée X de la souris.
 * @param mouseY Coordonnée Y de la souris.
 * @return 1 si la souris est au-dessus du bouton, sinon 0.
 */
int checkButtonHover(Button* button, int mouseX, int mouseY) {
    return (mouseX >= button->rect.x && mouseX <= button->rect.x + button->rect.w &&
            mouseY >= button->rect.y && mouseY <= button->rect.y + button->rect.h);
}
/**
 * @brief Libère les ressources du menu (textures SDL).
 * @param texture Texture principale à libérer.
 * @param textTexture Texture du texte à libérer.
 */
void libererRessourcesMenu(SDL_Texture* texture, SDL_Texture* textTexture) {
    if (texture) SDL_DestroyTexture(texture);
    if (textTexture) SDL_DestroyTexture(textTexture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (settingsTexture) SDL_DestroyTexture(settingsTexture);
    if(menuMusique) {
        Mix_FreeMusic(menuMusique);
        menuMusique = NULL;
    }
}
/**
 * @brief Recharge tous les menus du jeu.
 * @param menuFont Police utilisée pour le texte des menus.
 * @param renderer Renderer SDL utilisé pour créer les textures.
 */
void reloadAllMenus(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = NULL;

    for (int i = 0; i < 3; i++) {
        if (buttons[i].textTexture) SDL_DestroyTexture(buttons[i].textTexture);
        buttons[i].textTexture = NULL;
    }
    
    initMainMenu(menuFont, renderer);
    initSubMenu(menuFont, renderer);
    initLevelMenu(menuFont, renderer);
    initCoulMenu(menuFont, renderer);
    initSauvMenu(menuFont, renderer); 
}


/*----- MENU PRINCIPAL -----*/

/**
 * @brief Initialise le menu principal du jeu.
 *
 * Charge l’image de fond et les textures des boutons du menu principal selon le thème (clair ou sombre).
 * Crée trois boutons : "JOUER", "PARTIE SAUVEGARDEE", et "QUITTER" avec leurs couleurs et positions.
 *
 * @param menuFont Police utilisée pour le texte des boutons.
 * @param renderer Renderer SDL pour la création des textures.
 */
void initMainMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");

    settingsTexture = IMG_LoadTexture(renderer, "../assets/settings.png");
    int buttonWidth = 300;
    int buttonHeight = 70;

    buttons[0].rect = (SDL_Rect){ LARGEUR_MIN_FENETRE/2 - buttonWidth/2,  HAUTEUR_MIN_FENETRE*0.25, buttonWidth, buttonHeight };
    if(themeMode == 0){
        buttons[0].normalColor = (SDL_Color){71, 63, 52,255};
        buttons[0].hoverColor = (SDL_Color){139, 147, 84,255};
        buttons[0].isHovered = 0;
    }
    else {
        buttons[0].normalColor = (SDL_Color){42,45,35,255};
        buttons[0].hoverColor = (SDL_Color){76,98,66,255};
        buttons[0].isHovered = 0;
    }
    buttons[1].rect = (SDL_Rect){ LARGEUR_MIN_FENETRE/2 - buttonWidth/2, HAUTEUR_MIN_FENETRE*0.45, buttonWidth, buttonHeight };
    if(themeMode == 0){
        buttons[1].normalColor = (SDL_Color){71, 63, 52,255};
        buttons[1].hoverColor = (SDL_Color){139, 147, 84,255};
        buttons[1].isHovered = 0;
    }
    else {
        buttons[1].normalColor = (SDL_Color){42,45,35,255};
        buttons[1].hoverColor = (SDL_Color){76,98,66,255};
        buttons[1].isHovered = 0;
    }
    buttons[2].rect = (SDL_Rect){ LARGEUR_MIN_FENETRE/2 - buttonWidth/2, HAUTEUR_MIN_FENETRE*0.65, buttonWidth, buttonHeight };
    if(themeMode == 0){
        buttons[2].normalColor = (SDL_Color){71, 63, 52,255};
        buttons[2].hoverColor = (SDL_Color){139, 147, 84,255};
        buttons[2].isHovered = 0;
    }
    else {
        buttons[2].normalColor =  (SDL_Color){42,45,35,255};
        buttons[2].hoverColor =  (SDL_Color){76,98,66,255};
        buttons[2].isHovered = 0;  
    }
    const char* texts[3] = {"JOUER", "PARTIE SAUVEGARDEE", "QUITTER"};
    SDL_Surface* textSurface;
    for (int i = 0; i < 3; i++) {
        if(themeMode == 0) textSurface = TTF_RenderText_Solid(menuFont, texts[i], (SDL_Color){240, 229, 231,255});
        else textSurface = TTF_RenderText_Solid(menuFont, texts[i], (SDL_Color){203, 203, 203,255});
        buttons[i].textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        buttons[i].textRect = (SDL_Rect){ buttons[i].rect.x + (buttonWidth - textSurface->w)/2,
                                          buttons[i].rect.y + (buttonHeight - textSurface->h)/2,
                                          textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
    }
    settingsRect = (SDL_Rect){10,10,40,40};
}

/**
 * @brief Affiche le menu principal à l’écran.
 * @param renderer Renderer SDL pour dessiner les éléments.
 * @param menuFont Police utilisée pour le titre du menu.
 */
void renderMainMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!jouerMusique && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); // -1 = boucle infinie
        jouerMusique = 1;
    }
    if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52,255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35,255);
    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    if (settingsTexture != NULL)
        SDL_RenderCopy(renderer, settingsTexture, NULL, &settingsRect);
    SDL_Color titleColor;
    if( themeMode == 0) titleColor = (SDL_Color){240, 229, 231,255};
    else titleColor = (SDL_Color){203,203,203,255};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(menuFont, "BIENVENUE SUR OTHELLO", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { LARGEUR_MIN_FENETRE/2 - titleSurface->w/2, 50, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_DestroyTexture(titleTexture); 
    SDL_FreeSurface(titleSurface);
    for (int i = 0; i < 3; i++) {
        Button *button = &buttons[i];
        SDL_SetRenderDrawColor(renderer,
            button->isHovered ? button->hoverColor.r : button->normalColor.r,
            button->isHovered ? button->hoverColor.g : button->normalColor.g,
            button->isHovered ? button->hoverColor.b : button->normalColor.b,
            255);
        SDL_RenderFillRect(renderer, &button->rect);
        renderButtonText(renderer, button);
    }
}

/**
 * @brief Gère les événements dans le menu principal.
 *
 * Cette fonction intercepte les mouvements de souris, clics et événements système.
 * Elle détecte les interactions avec les boutons (Jouer, Partie sauvegardée, Quitter)
 * ainsi que l'icône des paramètres. Elle met à jour l’état du menu en conséquence.
 *
 * @param e Pointeur vers l'événement SDL.
 * @param quit Pointeur vers le drapeau de sortie du jeu.
 * @param renderer Le renderer SDL pour les actions de rendu.
 * @param font1 La police utilisée pour recharger le menu de sauvegarde.
 */
void handleMainMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;
        if (e->type == SDL_MOUSEMOTION) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            for (int i = 0; i < 3; i++) {
                buttons[i].isHovered = checkButtonHover(&buttons[i], mouseX, mouseY);
            }
        }
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX >= settingsRect.x && mouseX <= settingsRect.x + settingsRect.w &&
                mouseY >= settingsRect.y && mouseY <= settingsRect.y + settingsRect.h) {
                currentMenu = MENU_SETTINGS;
            }
            for (int i = 0; i < 3; i++) {
                if (buttons[i].isHovered) {
                    if (i == 0) {
                        currentMenu = MENU_SUB;
                    } else if (i == 1) {
                         // Recharger le menu
                        SDL_DestroyTexture(btnSupprimer.textTexture);
                        for (int i = 0; i < numSavedGames; i++) {
                            SDL_DestroyTexture(savedGames[i].button.textTexture);
                        }
                        initSauvMenu(font1, renderer);
                        currentMenu = MENU_SAUV_PARTIES;
                    } else if (i == 2) {
                        *quit = 1;
                    }
                }
            }
            if(currentMenu == MENU_MAIN && !jouerMusique && menuMusique) {
                Mix_PlayMusic(menuMusique, -1);
                jouerMusique = 1;
            }
        }
    }
}

/*---- MENU CHOIX NIVEAUX DE DIFFICULTÉ DE L'IA -----*/

/**
 * @brief Initialise le menu de sélection de la difficulté de l’IA.
 *
 * Crée trois boutons pour les niveaux "Facile", "Moyen" et "Difficile", place leur texte
 * au centre de chaque bouton, et adapte l’arrière-plan au thème.
 *
 * @param menuFont La police utilisée pour le texte des boutons.
 * @param renderer Renderer SDL utilisé pour créer les textures.
 */
 void initLevelMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");

    int buttonWidth = 300, buttonHeight = 70;
    int centerX = LARGEUR_MIN_FENETRE / 2 - buttonWidth / 2;

    niveau1 = (bouton_t){centerX, HAUTEUR_MIN_FENETRE * 0.30, buttonWidth, buttonHeight, NULL, {0}};
    niveau2 = (bouton_t){centerX, HAUTEUR_MIN_FENETRE * 0.50, buttonWidth, buttonHeight, NULL, {0}};
    niveau3 = (bouton_t){centerX, HAUTEUR_MIN_FENETRE * 0.70, buttonWidth, buttonHeight, NULL, {0}};

    const char* texts[3] = {"Facile", "Moyen", "Difficile"};
    bouton_t* niveaux[3] = {&niveau1, &niveau2, &niveau3};

    for (int i = 0; i < 3; i++) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, texts[i], (SDL_Color){240, 229, 231, 255});
        niveaux[i]->textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        niveaux[i]->textRect = (SDL_Rect){ niveaux[i]->x + (buttonWidth - textSurface->w) / 2,
                                           niveaux[i]->y + (buttonHeight - textSurface->h) / 2,
                                           textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
    }
}


/**
 * @brief Affiche le menu de sélection du niveau de difficulté.
 *
 * Dessine l’arrière-plan, les trois boutons de difficulté (Facile, Moyen, Difficile)
 * et le titre du menu en haut de l’écran. Adapte les couleurs selon le thème actuel.
 *
 * @param renderer Renderer SDL utilisé pour le rendu à l’écran.
 * @param menuFont Police utilisée pour le titre.
 */
 void renderLevelMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); // -1 = boucle infinie
    }
    if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52, 255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    bouton_t* niveaux[3] = {&niveau1, &niveau2, &niveau3};

    for (int i = 0; i < 3; i++) {
        SDL_Rect rect = {niveaux[i]->x, niveaux[i]->y, niveaux[i]->l, niveaux[i]->h};
        SDL_SetRenderDrawColor(renderer, 71, 63, 52, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderCopy(renderer, niveaux[i]->textTexture, NULL, &niveaux[i]->textRect);
    }
    SDL_Color textColor ;
    if(themeMode == 0) textColor = (SDL_Color){240, 229, 231, 255};
    else textColor = (SDL_Color){203, 203, 203, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, "CHOISISSEZ LA DIFFICULTE", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { LARGEUR_MIN_FENETRE / 2 - textSurface->w / 2, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/**
 * @brief Gère les événements dans le menu de difficulté.
 *
 * Réagit aux clics de souris sur les boutons de difficulté. Une fois un niveau sélectionné,
 * le menu passe à l’étape suivante (choix des pions). Gère également l’événement ESCAPE
 * pour revenir au menu précédent.
 *
 * @param e Pointeur vers l’événement SDL.
 * @param quit Pointeur vers le drapeau de sortie.
 */
void handleLevelMenuEvents(SDL_Event *e, int *quit) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
            currentMenu = MENU_SUB;

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (e->type == SDL_MOUSEBUTTONDOWN) {
            if (mouseX >= niveau1.x && mouseX <= niveau1.x + niveau1.l &&
                mouseY >= niveau1.y && mouseY <= niveau1.y + niveau1.h) {
                difficultyLevel = FACILE; // Facile
            } else if (mouseX >= niveau2.x && mouseX <= niveau2.x + niveau2.l &&
                       mouseY >= niveau2.y && mouseY <= niveau2.y + niveau2.h) {
                difficultyLevel = MOYEN; // Moyen
            } else if (mouseX >= niveau3.x && mouseX <= niveau3.x + niveau3.l &&
                       mouseY >= niveau3.y && mouseY <= niveau3.y + niveau3.h) {
                difficultyLevel = DIFFICILE; // Difficile
            }

            if (difficultyLevel >= 0) {
                printf("Mode de jeu: Joueur vs Ordi - Niveau %d\n", difficultyLevel + 1);
                currentMenu = MENU_COUL_PIONS;                
            }
        }
    }
}


/*------  MENU CHOIX DU JEUX (JOUEUR OU IA) --------*/
/**
 * @brief Initialise le sous-menu de choix du mode de jeu.
 *
 * Prépare les trois boutons : "Joueur vs Joueur", "Joueur vs Ordinateur" et "Jouer en ligne",
 * en adaptant les couleurs et le fond selon le thème choisi.
 *
 * @param menuFont La police utilisée pour le texte des boutons.
 * @param renderer Renderer SDL utilisé pour la création des textures.
 */
void initSubMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");
    bouton1.l = 400; bouton1.h = 100; bouton1.x = LARGEUR_MIN_FENETRE / 2 - bouton1.l / 2; bouton1.y = HAUTEUR_MIN_FENETRE*0.20;
    SDL_Surface* textSurface1;
    if(themeMode == 0)textSurface1 = TTF_RenderText_Solid(menuFont, "JOUEUR VS JOUEUR", (SDL_Color){240, 229, 231,255});
    else textSurface1 = TTF_RenderText_Solid(menuFont, "JOUEUR VS JOUEUR", (SDL_Color){203,203,203,255});
    bouton1.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface1);
    bouton1.textRect = (SDL_Rect){ bouton1.x + (bouton1.l - textSurface1->w)/2,
                                   bouton1.y + (bouton1.h - textSurface1->h)/2,
                                   textSurface1->w, textSurface1->h };
    SDL_FreeSurface(textSurface1);

    bouton3.l = 400; bouton3.h = 100; bouton3.x = LARGEUR_MIN_FENETRE / 2 - bouton1.l / 2; bouton3.y = HAUTEUR_MIN_FENETRE*0.40;
    SDL_Surface* textSurface3;
    if(themeMode == 0)textSurface3 = TTF_RenderText_Solid(menuFont, "JOUER EN LIGNE", (SDL_Color){240, 229, 231,255});
    else textSurface3 = TTF_RenderText_Solid(menuFont, "JOUER EN LIGNE", (SDL_Color){203,203,203,255});
    bouton3.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface3);
    bouton3.textRect = (SDL_Rect){ bouton3.x + (bouton3.l - textSurface3->w)/2,
                                   bouton3.y + (bouton3.h - textSurface3->h)/2,
                                   textSurface3->w, textSurface3->h };
    SDL_FreeSurface(textSurface3);

    bouton2.l = 400; bouton2.h = 100; bouton2.x = LARGEUR_MIN_FENETRE / 2 - bouton3.l / 2; bouton2.y = HAUTEUR_MIN_FENETRE*0.60; 
    SDL_Surface* textSurface2 ;
    if(themeMode == 0)textSurface2 = TTF_RenderText_Solid(menuFont, "JOUEUR VS ORDINATEUR", (SDL_Color){240, 229, 231,255});
    else textSurface2 = TTF_RenderText_Solid(menuFont, "JOUEUR VS ORDINATEUR", (SDL_Color){203,203,203,255});
    bouton2.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface2);
    bouton2.textRect = (SDL_Rect){ bouton2.x + (bouton2.l - textSurface2->w)/2,
                                   bouton2.y + (bouton2.h - textSurface2->h)/2,
                                   textSurface2->w, textSurface2->h };
    SDL_FreeSurface(textSurface2);
}

/**
 * @brief Affiche le sous-menu de sélection du mode de jeu.
 * @param renderer Renderer SDL pour dessiner les éléments du menu.
 * @param menuFont Police utilisée pour le titre du menu.
 */
void renderSubMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52,255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35,255);
    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_Rect rect1 = {bouton1.x, bouton1.y, bouton1.l, bouton1.h};
    SDL_Rect rect2 = {bouton2.x, bouton2.y, bouton2.l, bouton2.h};
    SDL_Rect rect3 = {bouton3.x, bouton3.y, bouton3.l, bouton3.h};
    if (themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52,255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35,255);
    SDL_RenderFillRect(renderer, &rect1);
    SDL_RenderFillRect(renderer, &rect2);
    SDL_RenderFillRect(renderer, &rect3);
    SDL_RenderCopy(renderer, bouton1.textTexture, NULL, &bouton1.textRect);
    SDL_RenderCopy(renderer, bouton2.textTexture, NULL, &bouton2.textRect);
    SDL_RenderCopy(renderer, bouton3.textTexture, NULL, &bouton3.textRect);
    SDL_Color textColor ;
    if (themeMode == 0)  textColor = (SDL_Color){240, 229, 231,255};
    else  textColor = (SDL_Color){203,203,203,255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, "CHOISIR LE MODE DE JEU", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { LARGEUR_MIN_FENETRE/2 - textSurface->w/2, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


/**
 * @brief Gère les événements dans le sous-menu.
 *
 * Permet de sélectionner un mode de jeu parmi "Joueur vs Joueur", "Joueur vs IA" et "Jouer en ligne".
 * Gère les clics de souris et la touche ESCAPE. Lance la partie ou redirige vers un autre menu.
 *
 * @param e Pointeur vers l’événement SDL.
 * @param quit Pointeur vers le drapeau de sortie du jeu.
 * @param renderer Renderer SDL utilisé pour démarrer une partie si nécessaire.
 * @param font1 Police principale pour le jeu.
 * @param font2 Police secondaire pour le jeu.
 */
void handleSubMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
            currentMenu = MENU_MAIN;
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = e->button.x, mouseY = e->button.y;
            if (mouseX >= bouton1.x && mouseX <= bouton1.x + bouton1.l &&
                mouseY >= bouton1.y && mouseY <= bouton1.y + bouton1.h) {
                gameMode = 1; // Joueur vs Joueur
                int gameReturnCode = JoueurVsJoueur(renderer, font1, font2);
                if (gameReturnCode == 1) {
                    currentMenu = MENU_MAIN;
                }
            } else if (mouseX >= bouton2.x && mouseX <= bouton2.x + bouton2.l &&
                       mouseY >= bouton2.y && mouseY <= bouton2.y + bouton2.h) {
                gameMode = 2; // Joueur vs Ordi
                currentMenu = MENU_LEVEL_IA; 
            }else if(mouseX >= bouton3.x && mouseX <= bouton3.x + bouton3.l &&
                mouseY >= bouton3.y && mouseY <= bouton3.y + bouton3.h){
                    currentMenu = MENU_RESEAU;
            }
        }
    }
}
/** 
* \fn initReseauMenu(TTF_Font* menuFont, SDL_Renderer* renderer);
* \brief Initialiser les boutons, couleurs de textes, les textes et l'arrière plan de menu de jeu en ligne
* \param menuFont Police utilisée pour l'affichage du menu de réseau.
* \param renderer Pointeur vers le renderer SDL utilisé pour dessiner.
*/
/*------------------MENU RESEAU-------------------------*/
void initReseauMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");
    bouton4.l = 400; bouton4.h = 100; bouton4.x = LARGEUR_MIN_FENETRE / 2 - bouton4.l / 2; bouton4.y = HAUTEUR_MIN_FENETRE*0.30;
    SDL_Surface* textSurface4;
    if(themeMode == 0)textSurface4 = TTF_RenderText_Solid(menuFont, "CREER UNE PARTIE", (SDL_Color){240, 229, 231,255});
    else textSurface4 = TTF_RenderText_Solid(menuFont, "CRÉER UNE PARTIE", (SDL_Color){203,203,203,255});
    bouton4.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface4);
    bouton4.textRect = (SDL_Rect){ bouton4.x + (bouton4.l - textSurface4->w)/2,
                                   bouton4.y + (bouton4.h - textSurface4->h)/2,
                                   textSurface4->w, textSurface4->h };
    SDL_FreeSurface(textSurface4);

    bouton5.l = 400; bouton5.h = 100; bouton5.x = LARGEUR_MIN_FENETRE / 2 - bouton4.l / 2; bouton5.y = HAUTEUR_MIN_FENETRE*0.60;
    SDL_Surface* textSurface5;
    if(themeMode == 0)textSurface5 = TTF_RenderText_Solid(menuFont, "REJOINDRE UNE PARTIE", (SDL_Color){240, 229, 231,255});
    else textSurface5 = TTF_RenderText_Solid(menuFont, "REJOINDRE UNE PARTIE", (SDL_Color){203,203,203,255});
    bouton5.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface5);
    bouton5.textRect = (SDL_Rect){ bouton5.x + (bouton5.l - textSurface5->w)/2,
                                   bouton5.y + (bouton5.h - textSurface5->h)/2,
                                   textSurface5->w, textSurface5->h };
    SDL_FreeSurface(textSurface5);
}
/** 
* \fn renderReseauMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
* \brief Rendre visible le menu de jeu en ligne.
* \param renderer Pointeur vers le renderer SDL utilisé pour dessiner.
* \param menuFont Police utilisée pour l'affichage du menu de réseau.
*/

void renderReseauMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!jouerMusique && menuMusique) {
        Mix_PlayMusic(menuMusique, -1);
        jouerMusique = 1;
    }
    if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52,255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35,255);
    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_Rect rect4 = {bouton4.x, bouton4.y, bouton4.l, bouton4.h};
    SDL_Rect rect5 = {bouton5.x, bouton5.y, bouton5.l, bouton5.h};
   
    if (themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52,255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35,255);
    SDL_RenderFillRect(renderer, &rect4);
    SDL_RenderFillRect(renderer, &rect5);
  
    SDL_RenderCopy(renderer, bouton4.textTexture, NULL, &bouton4.textRect);
    SDL_RenderCopy(renderer, bouton5.textTexture, NULL, &bouton5.textRect);
    
    SDL_Color textColor ;
    if (themeMode == 0)  textColor = (SDL_Color){240, 229, 231,255};
    else  textColor = (SDL_Color){203,203,203,255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, "CHOISIR LE MODE DE JEU", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { LARGEUR_MIN_FENETRE/2 - textSurface->w/2, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/** 
* \fn recuperer_ip();
* \brief Récupèrer l'adresse IP locale de la machine locale.
* \return une ip si le cas succès, NULL sinon
*/
char* recuperer_ip()
{
    char hostname[256];
    struct hostent *host;
    struct in_addr **adr;

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        if ((host = gethostbyname(hostname)) != NULL) {
            adr = (struct in_addr **)host->h_addr_list;
            if (*adr != NULL) {
                char *ip = malloc(INET_ADDRSTRLEN);
                if (ip != NULL) {
                    strncpy(ip, inet_ntoa(**adr), INET_ADDRSTRLEN);
                    ip[INET_ADDRSTRLEN - 1] = '\0'; 
                    return ip;
                }
            }
        }
    }
    return NULL; 
}

/**
 * \fn handleReseauMenuEvents(SDL_Event *e, int *quit);
 * \brief Gère les événements SDL pour le menu réseau.
 * - Lance un serveur si le bouton "Créer une partie" est cliqué.
 * - Change vers le menu de connexion à une partie si le bouton "Rejoindre une partie" est cliqué.
 *
 * \param e Pointeur vers l'événement SDL à traiter.
 * \param quit Pointeur vers la variable de contrôle principal pour quitter le programme.
 */
void handleReseauMenuEvents(SDL_Event *e, int *quit) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
            currentMenu = MENU_SUB;
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = e->button.x, mouseY = e->button.y;
            if (mouseX >= bouton4.x && mouseX <= bouton4.x + bouton4.l &&
                mouseY >= bouton4.y && mouseY <= bouton4.y + bouton4.h) {
                    ctx.client_socket = -1;
                    ctx.server_socket = -1;
                    memset(&ctx.client_addr, 0, sizeof(ctx.client_addr));
                    memset(&ctx.server_addr, 0, sizeof(ctx.server_addr));
                    ctx.serveur_info = NULL; 
                if (installer_serveur(&ctx) == 0) {
                    printf("\nServeur en attente de connexion...\n");
                    currentMenu = MENU_CREER_PARTIE_RESEAU;
                    serveurPret = 1;
                }
                
            } else if (mouseX >= bouton5.x && mouseX <= bouton5.x + bouton5.l &&
                       mouseY >= bouton5.y && mouseY <= bouton5.y + bouton5.h) {
                        ctx.client_socket = -1;
                        ctx.server_socket = -1;
                        memset(&ctx.client_addr, 0, sizeof(ctx.client_addr));
                        memset(&ctx.server_addr, 0, sizeof(ctx.server_addr));
                        ctx.serveur_info = NULL;
                        currentMenu = MENU_REJOINDRE_PARTIE_RESEAU;
            
            }
        }
    }
}

/*-----------LANCER UNE PARTIE-----------*/
/**
 * \fn renderCreerPartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont);
 * \brief Affiche le menu de création de partie réseau.
 *
 * Cette fonction rend à l'écran une interface graphique indiquant que le serveur est
 * en attente de connexion, en affichant l’adresse IP locale de la machine. 
 *
 * \param renderer Pointeur vers le renderer SDL utilisé pour dessiner l'écran.
 * \param menuFont Police utilisée pour le texte affiché à l'écran.
 */

void renderCreerPartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont){
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    char * ip = recuperer_ip();
    char message[256];
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");
    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_Color textColor ;
    if (themeMode == 0)  textColor = (SDL_Color){240, 229, 231,255};
    else  textColor = (SDL_Color){203,203,203,255};
    snprintf(message, sizeof(message), "Serveur en attente de connexion... | IP: %s", ip);
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, message, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { LARGEUR_MIN_FENETRE/2 - textSurface->w/2, 150, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
/**
 * \fn int thread_accepter_client();
 * \brief Fonction de thread chargée d'accepter une connexion client.
 *
 * Cette fonction est destinée à être exécutée dans un thread séparé. Elle appelle
 * `accepter_client(&ctx)` pour attendre une connexion entrante. En cas de succès,
 * une confirmation est affichée et une variable globale `clientAccepte` est mise à jour.
 * 
 * \return 0 à la fin de l'exécution du thread.
 */

int thread_accepter_client() {
    if (accepter_client(&ctx) == 0) {
        printf("Connexion acceptée !\n");
        clientAccepte = 1;
        
    } else {
        printf("Échec de la connexion.\n");
    }
    return 0;
}



/**
 * \fn renderRejoindrePartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont, const char* ipInput);
 * \brief Affiche le menu de connexion à une partie réseau.
 *
 * Cette fonction affiche un écran permettant à l'utilisateur de saisir l'adresse IP
 * du serveur auquel il souhaite se connecter. Elle affiche également une zone de saisie 
 * avec le texte en cours (`ipInput`) ainsi que la composition en cours (`composition`, si applicable).
 * Le fond et les couleurs s'adaptent au thème actuel (clair ou sombre).
 *
 * \param renderer Pointeur vers le renderer SDL utilisé pour dessiner à l'écran.
 * \param menuFont Police utilisée pour l'affichage des textes dans le menu.
 * \param ipInput Texte actuel saisi par l'utilisateur (IP cible du serveur).
 */
void renderRejoindrePartieMenu(SDL_Renderer* renderer, TTF_Font* menuFont, const char* ipInput) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    if (!textInputStarted) {
        SDL_StartTextInput();
        textInputStarted = 1;
    }

    if (themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");

    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Color textColor = (themeMode == 0) ? (SDL_Color){240, 229, 231,255} : (SDL_Color){203,203,203,255};

    SDL_Surface* promptSurface = TTF_RenderText_Solid(menuFont, "Veuillez entrer l'IP :", textColor);
    SDL_Texture* promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
    SDL_Rect promptRect = { LARGEUR_MIN_FENETRE/2 - promptSurface->w/2, 150, promptSurface->w, promptSurface->h };
    SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
    SDL_FreeSurface(promptSurface);
    SDL_DestroyTexture(promptTexture);

  
    SDL_Rect inputBox = { LARGEUR_MIN_FENETRE/2 - 150, 200, 300, 40 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &inputBox);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &inputBox);

   
    if (strlen(ipInput) > 0) {
        SDL_Surface* inputSurface = TTF_RenderText_Solid(menuFont, ipInput, (SDL_Color){0, 0, 0, 255});
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputTextRect = { inputBox.x + 5, inputBox.y + 5, inputSurface->w, inputSurface->h };
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputTextRect);
        SDL_FreeSurface(inputSurface);
        SDL_DestroyTexture(inputTexture);
    }

    if (strlen(composition) > 0) {
        SDL_Surface* compSurface = TTF_RenderText_Solid(menuFont, composition, (SDL_Color){100, 100, 100, 255});
        SDL_Texture* compTexture = SDL_CreateTextureFromSurface(renderer, compSurface);
        SDL_Rect compRect = { inputBox.x + 5, inputBox.y + 25, compSurface->w, compSurface->h };
        SDL_RenderCopy(renderer, compTexture, NULL, &compRect);
        SDL_FreeSurface(compSurface);
        SDL_DestroyTexture(compTexture);
    }

    SDL_RenderPresent(renderer);
}




/**
 * \fn handleRejoindrePartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
 * \brief Gère les événements SDL pour le menu "Rejoindre une partie réseau".
 *
 * Cette fonction intercepte les événements clavier et texte pour permettre à l'utilisateur :
 * - de saisir une adresse IP,
 * - de se connecter à un serveur
 *
 * En cas de validation (Entrée), elle tente de se connecter à l'IP saisie, puis lance le jeu si la connexion est réussie.
 *
 * \param e Pointeur vers l'événement SDL courant.
 * \param quit Pointeur vers le drapeau de sortie du programme.
 * \param renderer Pointeur vers le renderer SDL utilisé pour lancer le jeu après connexion.
 * \param font1 Police utilisée dans le jeu.
 * \param font2 Deuxième police utilisée dans le jeu.
 */

void handleRejoindrePartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    while (SDL_PollEvent(e)) {
        switch (e->type) {
            case SDL_QUIT:
                *quit = 1;
                break;

            case SDL_KEYDOWN:
                if (e->key.keysym.sym == SDLK_ESCAPE) {
                    currentMenu = MENU_RESEAU;
                    SDL_StopTextInput();
                    textInputStarted = 0;
                    ipInput[0] = '\0';
                } else if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(ipInput) > 0) {
                    ipInput[strlen(ipInput) - 1] = '\0';
                } else if (e->key.keysym.sym == SDLK_RETURN) {
                    if (connecter_server(&ctx) == 0) {
                        printf("Connexion réussie à %s !\n", ipInput);
                        client_joue_jeu(&ctx, window, renderer, font1, font2);
                        ipInput[0] = '\0';
                        SDL_StopTextInput();
                        textInputStarted = 0;
                    } else {
                        printf("Échec de connexion à %s\n", ipInput);
                        ipInput[0] = '\0';
                        currentMenu = MENU_RESEAU;
                        SDL_StopTextInput();
                        textInputStarted = 0;
                    }
                }
                break;

            case SDL_TEXTINPUT:
                if (strlen(ipInput) + strlen(e->text.text) < MAX_IP_LENGTH - 1) {
                    strcat(ipInput, e->text.text);
                }
                break;

            case SDL_TEXTEDITING:
                strncpy(composition, e->edit.text, MAX_IP_LENGTH - 1);
                composition[MAX_IP_LENGTH - 1] = '\0';
                cursor = e->edit.start;
                selection_len = e->edit.length;
                break;
        }
    }

    SDL_Delay(16);
}

/**
 * \fn andleCreerPartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
 * \brief Gère les événements SDL pour le menu "Créer une partie réseau".
 * \param e Pointeur vers l'événement SDL courant.
 * \param quit Pointeur vers la variable de contrôle de sortie principale.
 * \param renderer Pointeur vers le renderer SDL pour lancer le jeu après connexion.
 * \param font1 Première police utilisée dans le jeu.
 * \param font2 Deuxième police utilisée dans le jeu.
 */

void handleCreerPartieEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE){
            fermer_serveur_socket(&ctx);
            ctx.client_socket = -1;
            ctx.server_socket = -1;
            memset(&ctx.client_addr, 0, sizeof(ctx.client_addr));
            memset(&ctx.server_addr, 0, sizeof(ctx.server_addr));
            ctx.serveur_info = NULL;
            serveurPret = 0;
            threadLance = 0;
            currentMenu = MENU_RESEAU;}
        
    }
    if (serveurPret && !clientAccepte && !threadLance){
        SDL_CreateThread(thread_accepter_client, "ThreadClient", NULL);
        threadLance = 1;
    }

    if (clientAccepte) {
        serveur_joue_jeu(&ctx, window, renderer, font1, font2);
        clientAccepte = 0;
    }

}

/*------ PARAMETRES DU MENU -------*/
/**
 * @brief Affiche un texte à l'écran à une position donnée.
 *
 * Cette fonction utilise la SDL_ttf pour rendre une chaîne de caractères
 * sur une surface, puis la transforme en texture à afficher à l'écran.
 *
 * @param txt Texte à afficher.
 * @param x Position horizontale (en pixels) de l'affichage du texte.
 * @param y Position verticale (en pixels) de l'affichage du texte.
 * @param titleColor Couleur du texte (SDL_Color).
 * @param renderer Le renderer SDL utilisé pour l'affichage.
 * @param font La police (TTF_Font*) à utiliser pour le rendu du texte.
 */
void affiche_text(const char* txt, int x, int y, SDL_Color titleColor, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* helpSurface = TTF_RenderText_Solid(font, txt, titleColor);
    SDL_Texture* helpTexture = SDL_CreateTextureFromSurface(renderer, helpSurface);
    SDL_Rect helpRect = { x, y, helpSurface->w, helpSurface->h };
    SDL_RenderCopy(renderer, helpTexture, NULL, &helpRect);
    SDL_FreeSurface(helpSurface);
    SDL_DestroyTexture(helpTexture);
}

/**
 * @brief Affiche le menu des paramètres (volume, thèmes, retour).
 *
 * Cette fonction dessine l'écran des paramètres, y compris les barres de volume,
 * les boutons de sélection du thème (clair/sombre), et le bouton pour retourner au menu principal.
 *
 * @param renderer Le renderer SDL utilisé pour l'affichage.
 * @param menuFont La police utilisée pour les textes du menu.
 */
void renderSettingsMenu(SDL_Renderer *renderer, TTF_Font *menuFont) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    SDL_SetRenderDrawColor(renderer, 59, 53, 45, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color color = {240, 229, 231, 255};
    SDL_Color color1 = {59, 53, 45, 255};

    if (backgroundTexture != NULL) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }

    SDL_Surface* titleSurface = TTF_RenderText_Solid(menuFont, "PARAMETRES", color);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { LARGEUR_MIN_FENETRE / 2 - titleSurface->w / 2, 50, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Barre de volume musique
    affiche_text("Musique", 50, 190, color, renderer, menuFont);
    SDL_SetRenderDrawColor(renderer, 240, 229, 231, 255);
    SDL_RenderFillRect(renderer, &slidebar);
    // Positionner le "slider" en fonction du volume actuel
    slidehandler.x = 260 + (volume * 200 / MIX_MAX_VOLUME) - 5;
    SDL_RenderFillRect(renderer, &slidehandler);

    // Barre de volume effets sonores
    affiche_text("Effets Sonores", 50, 250, color, renderer, menuFont);
    SDL_SetRenderDrawColor(renderer, 240, 229, 231, 255);
    SDL_RenderFillRect(renderer, &slidebar2);
    // Positionner le "slider" en fonction du volume actuel
    slidehandler2.x = 260 + (soundVolume * 200 / MIX_MAX_VOLUME) - 5;
    SDL_RenderFillRect(renderer, &slidehandler2);

    // Boutons mode clair et mode sombre
    SDL_Rect modeClairRect = {40, 310, 150, 40}; 
    SDL_Rect modeSombreRect = {210, 310, 185, 40};

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);  
    SDL_RenderFillRect(renderer, &modeClairRect);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &modeSombreRect);

    affiche_text("Mode Clair", 50, 315, color1, renderer, menuFont);
    affiche_text("Mode Sombre", 220, 315, color, renderer, menuFont);

    // Bouton de retour au menu
    SDL_Rect retourMenuRect = {LARGEUR_MIN_FENETRE / 2 - 95, HAUTEUR_MIN_FENETRE - 100, 200, 50};
    SDL_SetRenderDrawColor(renderer, 180, 50, 50, 255);  
    SDL_RenderFillRect(renderer, &retourMenuRect);
    affiche_text("Retour", LARGEUR_MIN_FENETRE / 2 - 30, HAUTEUR_MIN_FENETRE - 85, color, renderer, menuFont);
}

/**
 * @brief Gère les événements dans le menu des paramètres.
 *
 * Cette fonction intercepte les événements utilisateur (clics souris, touches clavier),
 * et ajuste les éléments du menu des paramètres en conséquence :
 * - Ajustement du volume de la musique et des effets sonores.
 * - Changement de thème clair/sombre.
 * - Retour au menu principal.
 *
 * @param e L'événement SDL à traiter.
 * @param quit Pointeur vers une variable de contrôle pour quitter l'application.
 * @param renderer Le renderer SDL pour les éventuelles mises à jour graphiques.
 * @param menuFont La police utilisée pour l'affichage du texte.
 */
void handleSettingsMenuEvents(SDL_Event *e, int *quit, SDL_Renderer *renderer, TTF_Font *menuFont) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;

        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
            currentMenu = MENU_MAIN;

        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int x = e->button.x, y = e->button.y;

            // Ajustement du volume musique
            if (x >= 260 && x <= 460 && y >= 190 && y <= 220) {
                 // Convertit la position en valeur de volume
                 volume = (x - 260) * MIX_MAX_VOLUME / 200;
                 if (volume < 0) volume = 0;
                 if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
                 slidehandler.x = x - 5;
                 Mix_VolumeMusic(volume); // Applique le volume
            }

            // Ajustement du volume effets sonores
            if (x >= 260 && x <= 460 && y >= 250 && y <= 280) {
                // Convertit la position en valeur de volume
                soundVolume = (x - 260) * MIX_MAX_VOLUME / 200;
                if (soundVolume < 0) soundVolume = 0;
                if (soundVolume > MIX_MAX_VOLUME) soundVolume = MIX_MAX_VOLUME;
                
                slidehandler2.x = x - 5;
                Mix_Volume(-1, soundVolume); // Applique le volume
            }

            // Bouton mode clair
            if (x >= 40 && x <= 190 && y >= 310 && y <= 350){
                if (themeMode != 0) {
                    printf("Mode Clair activé\n");
                    themeMode = 0;
                    
                    // Libérer les textures existantes avant de recréer les menus
                    if (backgroundTexture) {
                        SDL_DestroyTexture(backgroundTexture);
                        backgroundTexture = NULL;
                    }
                    
                    // Réinitialiser tous les menus avec le nouveau thème
                    initMainMenu(menuFont, renderer);
                    initSubMenu(menuFont, renderer);
                    initLevelMenu(menuFont, renderer);
                    initCoulMenu(menuFont, renderer);
                }
                renderSettingsMenu(renderer, menuFont);
                SDL_RenderPresent(renderer);
            }

            // Bouton mode sombre
            if (x >= 210 && x <= 395 && y >= 310 && y <= 350){
                if (themeMode != 1) {
                    printf("Mode Sombre activé\n");
                    themeMode = 1;
                    reloadAllMenus(menuFont, renderer);
                    // Libérer les textures existantes avant de recréer les menus
                    if (backgroundTexture) {
                        SDL_DestroyTexture(backgroundTexture);
                        backgroundTexture = NULL;
                    }
                    
                    // Réinitialiser tous les menus avec le nouveau thème
                    initMainMenu(menuFont, renderer);
                    initSubMenu(menuFont, renderer);
                    initLevelMenu(menuFont, renderer);
                    initCoulMenu(menuFont, renderer);
                }
                renderSettingsMenu(renderer, menuFont);
                SDL_RenderPresent(renderer);
            }

            // Bouton de retour au menu
            if (x >= LARGEUR_MIN_FENETRE / 2 - 100 && x <= LARGEUR_MIN_FENETRE / 2 + 100 && 
                y >= HAUTEUR_MIN_FENETRE - 100 && y <= HAUTEUR_MIN_FENETRE - 50) {
                currentMenu = MENU_MAIN;
            }
        }
    }
}


/*------- CHOIX DE LA COULEUR DES PIONS--------*/

/**
 * \brief Initialise le menu de sélection de couleur des pions (noir ou blanc)
 * \param menuFont Police d'écriture
 * \param renderer Rendu SDL
 */
void initCoulMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");

    int buttonWidth = 300, buttonHeight = 70;
    int centerX = LARGEUR_MIN_FENETRE / 2 - buttonWidth / 2;

    couleur1 = (bouton_t){centerX, HAUTEUR_MIN_FENETRE * 0.30, buttonWidth, buttonHeight, NULL, {0}};
    couleur2 = (bouton_t){centerX, HAUTEUR_MIN_FENETRE * 0.50, buttonWidth, buttonHeight, NULL, {0}};

    const char* texts[2] = {"Blanc", "Noir"};
    bouton_t* couleurs[2] = {&couleur1, &couleur2};

    for (int i = 0; i < 2; i++) {
        SDL_Surface* textSurface;
        if(themeMode == 0) textSurface = TTF_RenderText_Solid(menuFont, texts[i], (SDL_Color){240, 229, 231, 255});
        else textSurface = TTF_RenderText_Solid(menuFont, texts[i], (SDL_Color){203,203,203, 255});
        couleurs[i]->textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        couleurs[i]->textRect = (SDL_Rect){ couleurs[i]->x + (buttonWidth - textSurface->w) / 2,
                                            couleurs[i]->y + (buttonHeight - textSurface->h) / 2,
                                           textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
    }
}

/**
 * \brief Affiche le menu de sélection de couleur des pions
 * \param renderer Rendu SDL
 * \param menuFont Police d'écriture
 */
void renderCoulMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52, 255);
    else SDL_SetRenderDrawColor(renderer, 42,45,35, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    bouton_t* couleurs[2] = {&couleur1, &couleur2};

    for (int i = 0; i < 2; i++) {
        SDL_Rect rect = {couleurs[i]->x, couleurs[i]->y, couleurs[i]->l, couleurs[i]->h};
        if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 71, 63, 52, 255);
        else SDL_SetRenderDrawColor(renderer, 42,45,35, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderCopy(renderer, couleurs[i]->textTexture, NULL, &couleurs[i]->textRect);
    }
    SDL_Color textColor;
    if(themeMode == 0)  textColor = (SDL_Color){240, 229, 231, 255};
    else  textColor = (SDL_Color){203, 203, 203, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, "CHOISISSEZ LA COULEUR", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { LARGEUR_MIN_FENETRE / 2 - textSurface->w / 2, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/**
 * \brief Gère les événements du menu de sélection de couleur des pions
 * \param e Événement SDL
 * \param quit Pointeur pour fermer la fenêtre
 * \param renderer Rendu SDL
 * \param font1 Police d'écriture
 * \param font2 Police d'écriture
 * \note Lance une partie contre l'IA avec la couleur sélectionnée
 */
void handleCoulMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT)
            *quit = 1;

        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (mouseX >= couleur1.x && mouseX <= couleur1.x + couleur1.l &&
                mouseY >= couleur1.y && mouseY <= couleur1.y + couleur1.h) {
                coulPion = blanc;
            } else if (mouseX >= couleur2.x && mouseX <= couleur2.x + couleur2.l &&
                       mouseY >= couleur2.y && mouseY <= couleur2.y + couleur2.h) {
                coulPion = noir;
            }

            if (coulPion == noir || coulPion == blanc) {
                printf("Couleur sélectionnée: %d\n", coulPion);
                int gameRetureCode = JoueurVsOrdi(renderer, font1, font2, &difficultyLevel, coulPion);
                if(gameRetureCode == 1){
                    currentMenu = MENU_MAIN;
                }
            }
        }
    }
}

/*------LISTE DES PARTIES SAUVEGARDÉES--------*/

/**
 * \brief Initialise le menu des parties sauvegardées
 * \param menuFont Police d'écriture
 * \param renderer Rendu SDL
 * \note Charge la liste des sauvegardes du fichier "liste_sauvegarde.txt"
 */
void initSauvMenu(TTF_Font* menuFont, SDL_Renderer* renderer) {
    for (int i = 0; i < numSavedGames; i++) {
        if (savedGames[i].button.textTexture != NULL) {
            SDL_DestroyTexture(savedGames[i].button.textTexture);
            savedGames[i].button.textTexture = NULL;
        }
    }
    if (btnSupprimer.textTexture != NULL) {
        SDL_DestroyTexture(btnSupprimer.textTexture);
        btnSupprimer.textTexture = NULL;
    }
    if (backgroundTexture != NULL) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = NULL;
    }

    if(themeMode == 0) backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloClair.png");
    else  backgroundTexture = IMG_LoadTexture(renderer, "../assets/othelloSombre.png");
    
    // Initialisation du bouton retour
    int buttonWidth = 200, buttonHeight = 50;
    boutonRetour.rect = (SDL_Rect){
        LARGEUR_MIN_FENETRE / 2 - buttonWidth / 2,  
        HAUTEUR_MIN_FENETRE - buttonHeight - 20, 
        buttonWidth, 
        buttonHeight
    };
    
    boutonRetour.normalColor = (SDL_Color){180, 50, 50, 255};
    boutonRetour.hoverColor = (SDL_Color){130, 50, 50, 255};
    boutonRetour.isHovered = 0;
    
    SDL_Surface* textSurface;
    if(themeMode == 0) 
        textSurface = TTF_RenderText_Solid(menuFont, "Retour", (SDL_Color){240, 229, 231, 255});
    else 
        textSurface = TTF_RenderText_Solid(menuFont, "Retour", (SDL_Color){203,203,203, 255});
    
    boutonRetour.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    boutonRetour.textRect = (SDL_Rect){ 
        boutonRetour.rect.x + (buttonWidth - textSurface->w)/2,
        boutonRetour.rect.y + (buttonHeight - textSurface->h)/2,
        textSurface->w, textSurface->h 
    };
    SDL_FreeSurface(textSurface);

    // Initialisation du bouton supprimer
    int deleteButtonWidth = 150, deleteButtonHeight = 40;
    btnSupprimer.rect = (SDL_Rect){ LARGEUR_MIN_FENETRE - deleteButtonWidth - 20, 20, deleteButtonWidth, deleteButtonHeight };
    
    btnSupprimer.normalColor = (SDL_Color){180, 50, 50, 255};
    btnSupprimer.hoverColor = (SDL_Color){130, 50, 50, 255};
    btnSupprimer.isHovered = 0;
    selection = -1;
    
    SDL_Surface* deleteTextSurface;
    if(themeMode == 0) 
        deleteTextSurface = TTF_RenderText_Solid(menuFont, "Supprimer", (SDL_Color){240, 229, 231, 255});
    else 
        deleteTextSurface = TTF_RenderText_Solid(menuFont, "Supprimer", (SDL_Color){203,203,203, 255});
    
    btnSupprimer.textTexture = SDL_CreateTextureFromSurface(renderer, deleteTextSurface);
    btnSupprimer.textRect = (SDL_Rect){ 
        btnSupprimer.rect.x + (deleteButtonWidth - deleteTextSurface->w)/2,
        btnSupprimer.rect.y + (deleteButtonHeight - deleteTextSurface->h)/2,
        deleteTextSurface->w, deleteTextSurface->h 
    };
    SDL_FreeSurface(deleteTextSurface);

    numSavedGames = 0;
    FILE* fichier = fopen(FICHIER_SAUVEGARDE, "r");
    
    if (fichier != NULL) {
        char ligne[MAX_NOM_FICHIER];
        while (fgets(ligne, sizeof(ligne), fichier) != NULL && numSavedGames < MAX_SAUVEGARDE) {
            ligne[strcspn(ligne, "\n")] = 0;
            
            if (strlen(ligne) > 0) {
                strncpy(savedGames[numSavedGames].filename, ligne, MAX_NOM_FICHIER - 1);
                savedGames[numSavedGames].filename[MAX_NOM_FICHIER - 1] = '\0';
                
                int buttonWidth = 400, buttonHeight = 50;
                int centerX = LARGEUR_MIN_FENETRE / 2 - buttonWidth / 2;
                int yPos = 150 + numSavedGames * 60;
                
                savedGames[numSavedGames].button.rect = (SDL_Rect){
                    centerX, yPos, buttonWidth, buttonHeight
                };
                
                if(themeMode == 0) {
                    savedGames[numSavedGames].button.normalColor = (SDL_Color){71, 63, 52, 255};
                    savedGames[numSavedGames].button.hoverColor = (SDL_Color){139, 147, 84, 255};
                } else {
                    savedGames[numSavedGames].button.normalColor = (SDL_Color){42, 45, 35, 255};
                    savedGames[numSavedGames].button.hoverColor = (SDL_Color){76, 98, 66, 255};
                }
                
                savedGames[numSavedGames].button.isHovered = 0;
                
                SDL_Surface* textSurface;
                if(themeMode == 0) 
                    textSurface = TTF_RenderText_Solid(menuFont, ligne, (SDL_Color){240, 229, 231, 255});
                else 
                    textSurface = TTF_RenderText_Solid(menuFont, ligne, (SDL_Color){203,203,203, 255});
                
                savedGames[numSavedGames].button.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                savedGames[numSavedGames].button.textRect = (SDL_Rect){ 
                    centerX + (buttonWidth - textSurface->w)/2,
                    yPos + (buttonHeight - textSurface->h)/2,
                    textSurface->w, textSurface->h 
                };
                SDL_FreeSurface(textSurface);
                
                numSavedGames++;
            }
        }
        fclose(fichier);
    }
}

/**
 * \brief Affiche le menu des parties sauvegardées
 * \param renderer Rendu SDL pour le dessin
 * \param menuFont Police d'écriture
 * \note Affiche la liste des parties sauvegardées
 */
void renderSauvMenu(SDL_Renderer* renderer, TTF_Font* menuFont) {
    if(!Mix_PlayingMusic() && menuMusique) {
        Mix_PlayMusic(menuMusique, -1); 
    }
    if(themeMode == 0) 
        SDL_SetRenderDrawColor(renderer, 71, 63, 52, 255);
    else 
        SDL_SetRenderDrawColor(renderer, 42, 45, 35, 255);

    SDL_RenderClear(renderer);
    if (backgroundTexture != NULL)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    
    SDL_Color textColor;
    if(themeMode == 0)  
        textColor = (SDL_Color){240, 229, 231, 255};
    else  
        textColor = (SDL_Color){203, 203, 203, 255};
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, "PARTIES SAUVEGARDEES", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { 
        LARGEUR_MIN_FENETRE / 2 - textSurface->w / 2, 
        50, 
        textSurface->w, 
        textSurface->h 
    };
    if(textTexture != NULL){
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }    
    for (int i = 0; i < numSavedGames; i++) {
        if (savedGames[i].button.isHovered) {
            SDL_SetRenderDrawColor(renderer, 
                savedGames[i].button.hoverColor.r,
                savedGames[i].button.hoverColor.g,
                savedGames[i].button.hoverColor.b,
                savedGames[i].button.hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(renderer, 
                savedGames[i].button.normalColor.r,
                savedGames[i].button.normalColor.g,
                savedGames[i].button.normalColor.b,
                savedGames[i].button.normalColor.a);
        }
        
        SDL_RenderFillRect(renderer, &savedGames[i].button.rect);
        SDL_RenderCopy(renderer, savedGames[i].button.textTexture, NULL, &savedGames[i].button.textRect);
    }
    //Rendu du bouton de retour
    SDL_SetRenderDrawColor(renderer, 
        boutonRetour.isHovered ? boutonRetour.hoverColor.r : boutonRetour.normalColor.r,
        boutonRetour.isHovered ? boutonRetour.hoverColor.g : boutonRetour.normalColor.g,
        boutonRetour.isHovered ? boutonRetour.hoverColor.b : boutonRetour.normalColor.b,
        255);
    SDL_RenderFillRect(renderer, &boutonRetour.rect);
    SDL_RenderCopy(renderer, boutonRetour.textTexture, NULL, &boutonRetour.textRect);

      // Rendu du bouton supprimer (seulement si une partie est sélectionnée)
    if (selection != -1) {
        SDL_SetRenderDrawColor(renderer, 
            btnSupprimer.isHovered ? btnSupprimer.hoverColor.r : btnSupprimer.normalColor.r,
            btnSupprimer.isHovered ? btnSupprimer.hoverColor.g : btnSupprimer.normalColor.g,
            btnSupprimer.isHovered ? btnSupprimer.hoverColor.b : btnSupprimer.normalColor.b,
            255);
        SDL_RenderFillRect(renderer, &btnSupprimer.rect);
        SDL_RenderCopy(renderer, btnSupprimer.textTexture, NULL, &btnSupprimer.textRect);
    }
}

/**
 * \brief Gère les événements du menu des parties sauvegardées
 * \param e Événement SDL
 * \param quit Pointeur pour fermer la fenêtre
 * \param renderer Rendu SDL
 * \param font1 Police d'écriture
 * \param font2 Police d'écriture
 * \note Permet de charger ou supprimer des sauvegardes
 */
void handleSauvMenuEvents(SDL_Event *e, int *quit, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT) {
            *quit = 1;
        }
        
        if (e->type == SDL_MOUSEMOTION) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            boutonRetour.isHovered = checkButtonHover(&boutonRetour, mouseX, mouseY);
            
            for (int i = 0; i < numSavedGames; i++) {
                savedGames[i].button.isHovered = checkButtonHover(&savedGames[i].button, mouseX, mouseY);
            }

            if (selection != -1) {
                btnSupprimer.isHovered = checkButtonHover(&btnSupprimer, mouseX, mouseY);
            }
        }
        
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Vérifier clique sur bouton retour
            if (checkButtonHover(&boutonRetour, mouseX, mouseY)) {
                currentMenu = MENU_MAIN;
                return;
            }
            
            // boutons de sauvegarde
            for (int i = 0; i < numSavedGames; i++) {
                if (checkButtonHover(&savedGames[i].button, mouseX, mouseY)) {
                    // Si clic simple, charger la partie
                    if (e->button.button == SDL_BUTTON_LEFT) {
                        printf("Chargement de la partie: %s\n", savedGames[i].filename);
                        // Chargement temporaire du jeu pour récupérer le mode 
                        t_case tempJeu[N][N];
                        t_case tempJoueur;
                        t_niveau tempMode;
                        int tempTempsnoir;
                        int tempTempsblanc;
                        if (chargerJeu(tempJeu, &tempJoueur, &tempMode, savedGames[i].filename, &tempTempsnoir, &tempTempsblanc)) {
                            if (tempMode == 0) {
                                // Mode Joueur vs Joueur
                                int gameReturnCode = JoueurVsJoueurSauv(renderer, font1, font2, savedGames[i].filename);
                                if(gameReturnCode == 1) {
                                    currentMenu = MENU_MAIN;
                                    return; 
                                }
                            } else {
                                // Mode Joueur vs IA
                                int gameReturnCode = JoueurVsOrdiSauv(renderer, font1, font2, &tempMode, coulPion, savedGames[i].filename);
                                if(gameReturnCode == 1) {
                                    currentMenu = MENU_MAIN;
                                    return;
                                }
                            }
                        }
                        return;
                    }
                    // Si clic droit, sélectionner pour supprimer
                    else if (e->button.button == SDL_BUTTON_RIGHT) {
                        if (selection != -1) {
                            if(themeMode == 0) 
                                savedGames[selection].button.normalColor = (SDL_Color){71, 63, 52, 255};
                            else 
                                savedGames[selection].button.normalColor = (SDL_Color){42, 45, 35, 255};
                        }
                        
                        selection = i;
                        savedGames[i].button.normalColor = (SDL_Color){130, 50, 50, 255};
                    }
                }
            }
            
            // Gestion clic bouton supprimer
            if (selection != -1 && 
                checkButtonHover(&btnSupprimer, mouseX, mouseY) && e->button.button == SDL_BUTTON_LEFT) {
                
                printf("Suppression de la partie: %s\n", savedGames[selection].filename);
                
                if(themeMode == 0) 
                    savedGames[selection].button.normalColor = (SDL_Color){71, 63, 52, 255};
                else 
                    savedGames[selection].button.normalColor = (SDL_Color){42, 45, 35, 255};
                
                supprimer_sauvegarde(savedGames[selection].filename);

                 // Recharger le menu
                if(btnSupprimer.textTexture) SDL_DestroyTexture(btnSupprimer.textTexture);
                for (int i = 0; i < numSavedGames; i++) {
                    if(savedGames[i].button.textTexture){
                        SDL_DestroyTexture(savedGames[i].button.textTexture);
                    }
                }
                initSauvMenu(font1, renderer);
                
                selection = -1;
            }
            
            // Désélection si on clique ailleurs
            if (selection != -1 && 
                !checkButtonHover(&btnSupprimer, mouseX, mouseY) &&
                !checkButtonHover(&savedGames[selection].button, mouseX, mouseY)) {
                
                if(themeMode == 0)savedGames[selection].button.normalColor = (SDL_Color){71, 63, 52, 255};
                else savedGames[selection].button.normalColor = (SDL_Color){42, 45, 35, 255};
                
                    selection = -1;
            }
        }
    }
}



