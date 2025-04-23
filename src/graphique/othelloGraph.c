#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../jeu/fonc_jeu.h" 
#include "../jeu/fonc_sauv.h" 
#include "../IA/fonc_ia.h" 
#include "menus.h" 
#include "plateau.h" 

/**
* \file othelloGraph.c
* \brief Fonction main.
* \author Nasreddine Biya, Alexandra Péan
*/

/**
 * @brief Fonction principale du programme.
 *
 * Cette fonction initialise les composants SDL nécessaires,
 * configure les menus, et entre dans une boucle principale qui gère l'affichage et les événements.
 * Elle permet également de gérer les différentes options de menus (principal, réseau, paramètres, etc.)
 * et lance la partie lorsque l'utilisateur choisit de jouer.
 *
 * @return EXIT_SUCCESS si le programme se termine correctement, EXIT_FAILURE en cas d'erreur.
 */
int main() {
    SDL_Renderer* renderer = NULL;
    TTF_Font* font1 = NULL;
    TTF_Font* font2 = NULL;

    DimensionsJeu dims = {
        .largeurFenetre = LARGEUR_MIN_FENETRE,
        .hauteurFenetre = HAUTEUR_MIN_FENETRE
    };
    initialiserDimensions(&dims);
    
    if (!initialiserSDL(&window, &renderer, &font1, &font2, &dims)) {
        return EXIT_FAILURE;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer n'a pas pu être initialisé. Erreur SDL_mixer : %s\n", Mix_GetError());
    }

    sonPion = Mix_LoadWAV("../assets/Pion.wav");
    if(sonPion == NULL) {
        fprintf(stderr, "Erreur chargement son: %s\n", Mix_GetError());
    }
    
    menuMusique = Mix_LoadMUS("../assets/menu.wav");
    if(menuMusique == NULL) {
        fprintf(stderr, "Erreur chargement son: %s\n", Mix_GetError());
    }

    if(menuMusique) {
        Mix_PlayMusic(menuMusique, -1);
        jouerMusique = 1; 
        volume = MIX_MAX_VOLUME / 4;  // 1/4 du volume max
        Mix_VolumeMusic(volume);
    }
    // Position initiale du slider
    slidehandler.x = 260 + (volume * 200 / MIX_MAX_VOLUME) - 5;

    initMainMenu(font1, renderer);
    initSubMenu(font1, renderer);
    initLevelMenu(font1, renderer) ;
    initCoulMenu(font1, renderer);
    initSauvMenu(font1, renderer);
    initReseauMenu(font1, renderer);
    
    int quit = 0;
    SDL_Event e;
    currentMenu = MENU_MAIN;

    SDL_SetWindowResizable(window, SDL_FALSE);
    
    while (!quit) {
        switch (currentMenu) {
            case MENU_MAIN:
                handleMainMenuEvents(&e, &quit, renderer, font1);
                renderMainMenu(renderer, font2);
                break;
                
            case MENU_SUB:
                handleSubMenuEvents(&e, &quit, renderer, font1, font2);
                renderSubMenu(renderer, font2);
                break;

            case MENU_RESEAU:
                handleReseauMenuEvents(&e, &quit);
                renderReseauMenu(renderer, font2);
                break;

            case MENU_CREER_PARTIE_RESEAU:
                
                handleCreerPartieEvents(&e, &quit, renderer, font1, font2);
                renderCreerPartieMenu(renderer, font2);
                
                break;
            case MENU_REJOINDRE_PARTIE_RESEAU:
            if (!textInputStarted) {
                SDL_StartTextInput();
                textInputStarted = 1;
                ipInput[0] = '\0'; // Clear previous IP
            }
            handleRejoindrePartieEvents(&e, &quit, renderer, font1, font2);
            renderRejoindrePartieMenu(renderer, font2, ipInput);
                
                break;
            case MENU_SETTINGS:
                handleSettingsMenuEvents(&e,&quit, renderer, font1);
                renderSettingsMenu(renderer, font2);
                break;

            case MENU_LEVEL_IA:
                handleLevelMenuEvents(&e, &quit);
                renderLevelMenu(renderer, font2);
                break;

            case MENU_COUL_PIONS:
                handleCoulMenuEvents(&e, &quit, renderer, font1, font2);
                renderCoulMenu(renderer, font2);
                break;

            case MENU_SAUV_PARTIES:
                handleSauvMenuEvents(&e, &quit, renderer, font1, font2);
                renderSauvMenu(renderer, font1); 
                break;

            case MENU_PLAY: {
                int result = JoueurVsJoueur(renderer, font1, font2); 
                if (result == 1) {
                    currentMenu = MENU_MAIN; 
                    renderMainMenu(renderer, font2); 
                }
                break;
            }
             default:
                break; 
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 60 FPS
    }
    nettoyerRessources(window, renderer, font1, font2);
    return EXIT_SUCCESS;
}