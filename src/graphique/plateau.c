#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "../jeu/fonc_jeu.h" 
#include "../jeu/fonc_sauv.h" 
#include "../IA/fonc_ia.h" 
#include "plateau.h" 

/**
* \file plateau.c
* \brief Définition des fonctions pour l'affichage du plateau.
* \author Alexandra Péan
*/

SDL_Window* window = NULL;

/**
* \fn dessinerPion(SDL_Renderer *renderer, int centreX, int centreY, int rayon, SDL_Color couleur)
* \brief Dessine un pion sur le plateau
* \param renderer Le rendu SDL
* \param centreX Coordonnée X du centre
* \param centreY Coordonnée Y du centre
* \param rayon Rayon du pion
* \param couleur Couleur du pion (noir ou blanc)
*/
void dessinerPion(SDL_Renderer *renderer, int centreX, int centreY, int rayon, SDL_Color couleur) {
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    for (int y = -rayon; y <= rayon; y++) {
        for (int x = -rayon; x <= rayon; x++) {
            // Vérifie si le point (x, y) est dans le cercle
            if (x*x + y*y <= rayon*rayon) {
                SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
            }
        }
    }
}
    

/**
* \fn afficherPlateau(SDL_Renderer *renderer, t_case jeu[N][N], int tailleCase, int posX, int posY)
* \brief Affiche le plateau de jeu
* \param renderer Le rendu SDL
* \param jeu Matrice du jeu
* \param tailleCase Taille d'une case en pixels
* \param posX Position X du plateau
* \param posY Position Y du plateau
*/
void afficherPlateau(SDL_Renderer *renderer, t_case jeu[N][N], int tailleCase, int posX, int posY) {
    SDL_Color couleurs[2];
    if(themeMode == 0) {
        couleurs[0] = (SDL_Color){105, 93, 77, 255};
        couleurs[1] = (SDL_Color){214, 198, 201, 255};
    } else {
        couleurs[0] = (SDL_Color){203, 203, 203, 255};
        couleurs[1] = (SDL_Color){64, 64, 64, 255};
    }
    SDL_Color coulNoir = {0, 0, 0, 255};
    SDL_Color coulBlanc = {255, 255, 255, 255};

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            //dessine les cases du plateau
            SDL_Rect caseRect = {posX + j * tailleCase, posY + i * tailleCase, tailleCase, tailleCase};
            SDL_SetRenderDrawColor(renderer, couleurs[(i + j) % 2].r, couleurs[(i + j) % 2].g, couleurs[(i + j) % 2].b, couleurs[(i + j) % 2].a);
            SDL_RenderFillRect(renderer, &caseRect);

            if (jeu[i][j] == noir) {
                dessinerPion(renderer, caseRect.x + tailleCase / 2, caseRect.y + tailleCase / 2, tailleCase / 3, coulNoir);
            } else if (jeu[i][j] == blanc) {
                dessinerPion(renderer, caseRect.x + tailleCase / 2, caseRect.y + tailleCase / 2, tailleCase / 3, coulBlanc);
            }
        }
    }
    // Dessiner le cadre autour du plateau
    SDL_SetRenderDrawColor(renderer, couleurs[1].r, couleurs[1].g, couleurs[1].b, couleurs[1].a);
    SDL_Rect cadre = {posX - 2, posY - 2, N * tailleCase + 4, N * tailleCase + 4};
    SDL_RenderDrawRect(renderer, &cadre);

}

/**
* \fn gererClics(SDL_Event *e, t_case jeu[N][N], t_case *joueurActuel, int tailleCase, int positionPlateauX, int positionPlateauY, int *temps_noir, int *temps_blanc, time_t *dernier_tour)
* \brief Gère les clics sur le plateau
* \param e Événement SDL
* \param jeu Matrice du jeu
* \param joueurActuel Pointeur vers le joueur actuel
* \param tailleCase Taille d'une case
* \param positionPlateauX Position X du plateau
* \param positionPlateauY Position Y du plateau
* \param temps_noir Pointeur vers le temps restant du joueur noir
* \param temps_blanc Pointeur vers le temps restant du joueur blanc
* \param dernier_tour Pointeur vers le timestamp du dernier tour
*/
void gererClics(SDL_Event *e, t_case jeu[N][N], t_case *joueurActuel, int tailleCase, int positionPlateauX, int positionPlateauY, int *temps_noir, int *temps_blanc, time_t *dernier_tour) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        int ligne = (y - positionPlateauY) / tailleCase;
        int col = (x - positionPlateauX) / tailleCase;

        if (ligne >= 0 && ligne < N && col >= 0 && col < N && jeu[ligne][col] == vide && coup_valide(jeu, ligne, col, *joueurActuel)) {

            if(sonPion != NULL) {
                Mix_PlayChannel(-1, sonPion, 0);
            }
            jeu[ligne][col] = *joueurActuel;
            retourner_pions(jeu, ligne, col, *joueurActuel);

            //Calcule le chrono du joueur
            int temps_ecoule = (int)(time(NULL) - *dernier_tour); 
            if (*joueurActuel == noir) {
                *temps_noir -= temps_ecoule;
            } else {
                *temps_blanc -= temps_ecoule;
            }
            
            //Changement de joueur
            *joueurActuel = (*joueurActuel == noir) ? blanc : noir;

            if (!peut_jouer(jeu, *joueurActuel)) {
                *joueurActuel = (*joueurActuel == noir) ? blanc : noir;
            }

            // Réinitialisation du chrono pour le nouveau joueur
            *dernier_tour = time(NULL);
            
  
        }
        
     
    }
}

/**
* \fn afficherScore(SDL_Renderer* renderer, TTF_Font* font1,TTF_Font* font2, int scoreNoir, int scoreBlanc, int hauteurFenetre, int largeurFenetre, int rayonPion)
* \brief Affiche les scores des joueurs
* \param renderer Le rendu SDL
* \param font1 Police d'écriture pour les scores
* \param font2 Police d'écriture pour les noms des joueurs
* \param scoreNoir Score du joueur noir
* \param scoreBlanc Score du joueur blanc
* \param hauteurFenetre Hauteur de la fenêtre
* \param largeurFenetre Largeur de la fenêtre
* \param rayonPion Rayon des pions affichés
*/
void afficherScore(SDL_Renderer* renderer, TTF_Font* font1,TTF_Font* font2, int scoreNoir, int scoreBlanc, int hauteurFenetre, int largeurFenetre, int rayonPion) {
    SDL_Color couleurPion, couleurScore, couleurJoueur;
    if(themeMode == 0){
        couleurPion = (SDL_Color){71, 63, 52, 255};
        couleurScore = (SDL_Color){240, 229, 231, 255};
        couleurJoueur = (SDL_Color){71, 63, 52, 255};
    }
    else{
        couleurPion = (SDL_Color){42,45,35, 255};
        couleurScore = (SDL_Color){203,203,203, 255};
        couleurJoueur = (SDL_Color){203,203,203, 255};
    }

    // Positionnement des cercles
    int positionPionNoirX = largeurFenetre*0.125; 
    int positionPionNoirY = hauteurFenetre / 2;

    int positionPionBlancX = largeurFenetre*0.875;
    int positionPionBlancY = hauteurFenetre / 2;

    dessinerPion(renderer, positionPionNoirX, positionPionNoirY, rayonPion, couleurPion); 
    dessinerPion(renderer, positionPionBlancX, positionPionBlancY, rayonPion, couleurPion);

    // Affichage des scores
    char texteScoreNoir[50];
    char texteScoreBlanc[50];
    sprintf(texteScoreNoir, "%d", scoreNoir);
    sprintf(texteScoreBlanc, "%d", scoreBlanc);

    // Affichage de la couleur des pions
    char texteNoir[50];
    char texteBlanc[50];
    sprintf(texteNoir, "NOIR");
    sprintf(texteBlanc, "BLANC");

    // Création des surfaces et textures pour les scores
    SDL_Surface* surfaceScoreNoir = TTF_RenderText_Solid(font1, texteScoreNoir, couleurScore);
    SDL_Surface* surfaceScoreBlanc = TTF_RenderText_Solid(font1, texteScoreBlanc, couleurScore);

    SDL_Texture* textureScoreNoir = SDL_CreateTextureFromSurface(renderer, surfaceScoreNoir);
    SDL_Texture* textureScoreBlanc = SDL_CreateTextureFromSurface(renderer, surfaceScoreBlanc);

    // Création des surfaces et textures pour l'affichage des couleurs
    SDL_Surface* surfaceNoir = TTF_RenderText_Solid(font2, texteNoir, couleurJoueur);
    SDL_Surface* surfaceBlanc = TTF_RenderText_Solid(font2, texteBlanc, couleurJoueur);

    SDL_Texture* textureNoir = SDL_CreateTextureFromSurface(renderer, surfaceNoir);
    SDL_Texture* textureBlanc = SDL_CreateTextureFromSurface(renderer, surfaceBlanc);

    // Positionnement des scores
    int posScoreNoirX = (largeurFenetre*0.125) - surfaceScoreNoir->w / 2;
    int posScoreNoirY = positionPionNoirY - surfaceScoreNoir->h / 2;

    int posScoreBlancX = (largeurFenetre*0.875)- surfaceScoreBlanc->w / 2;
    int posScoreBlancY = positionPionBlancY - surfaceScoreBlanc->h / 2;

    // Postionnement des couleurs 
    int posNoirX = largeurFenetre*0.125- surfaceNoir->w / 2;
    int posNoirY = hauteurFenetre*0.35;

    int posBlancX =largeurFenetre*0.875 - surfaceBlanc->w / 2;
    int posBlancY = hauteurFenetre*0.35;

    // Affichage des scores
    SDL_Rect posScoreNoir = {posScoreNoirX, posScoreNoirY, surfaceScoreNoir->w, surfaceScoreNoir->h};
    SDL_Rect posScoreBlanc = {posScoreBlancX, posScoreBlancY, surfaceScoreBlanc->w, surfaceScoreBlanc->h};
    //Affichage des couleurs 
    SDL_Rect posNoir = {posNoirX, posNoirY, surfaceNoir->w, surfaceNoir->h};
    SDL_Rect posBlanc = {posBlancX, posBlancY, surfaceBlanc->w, surfaceBlanc->h};

    SDL_RenderCopy(renderer, textureScoreNoir, NULL, &posScoreNoir);
    SDL_RenderCopy(renderer, textureScoreBlanc, NULL, &posScoreBlanc);
    SDL_RenderCopy(renderer, textureNoir, NULL, &posNoir);
    SDL_RenderCopy(renderer, textureBlanc, NULL, &posBlanc);

    // Nettoyage des surfaces et textures
    SDL_FreeSurface(surfaceScoreNoir);
    SDL_FreeSurface(surfaceScoreBlanc);
    SDL_DestroyTexture(textureScoreNoir);
    SDL_DestroyTexture(textureScoreBlanc);
    SDL_FreeSurface(surfaceNoir);
    SDL_FreeSurface(surfaceBlanc);
    SDL_DestroyTexture(textureNoir);
    SDL_DestroyTexture(textureBlanc);
}

/**
* \fn affichageTourJoueur(SDL_Renderer* renderer, TTF_Font* font, int largeurFenetre, int posYPlateau, t_case joueurActuel)
* \brief Affiche le tour du joueur actuel
* \param renderer Le rendu SDL
* \param font Police d'écriture
* \param largeurFenetre Largeur de la fenêtre
* \param posYPlateau Position Y du plateau
* \param joueurActuel Joueur dont c'est le tour
*/
void affichageTourJoueur(SDL_Renderer* renderer, TTF_Font* font, int largeurFenetre, int posYPlateau, t_case joueurActuel) {
    SDL_Color couleurNoir , couleurBlanc;
    if(themeMode == 0){
        couleurNoir = (SDL_Color){71, 63, 52, 255};
        couleurBlanc = (SDL_Color){240, 229, 231, 255}; 
    }
    else {
        couleurNoir = (SDL_Color){42,45,35,255};
        couleurBlanc = (SDL_Color){203,203,203, 255}; 
    }
    
    char texte[50];
    sprintf(texte, "TOUR DU JOUEUR ");

    // Création surface et texture pour "Tour du joueur"
    SDL_Surface* surfaceTour = TTF_RenderText_Solid(font, texte, themeMode == 0 ? couleurNoir : couleurBlanc);
    SDL_Texture* textureTour = SDL_CreateTextureFromSurface(renderer, surfaceTour);

    char texteJoueur[50];
    sprintf(texteJoueur, "%s", joueurActuel == noir ? "NOIR" : "BLANC");

    SDL_Color couleurJoueur = joueurActuel == noir ? couleurNoir : couleurBlanc;

    // Création surface et texture pour "noir" ou "blanc"
    SDL_Surface* surfaceJoueur = TTF_RenderText_Solid(font, texteJoueur, couleurJoueur);
    SDL_Texture* textureJoueur = SDL_CreateTextureFromSurface(renderer, surfaceJoueur);

    // Positionner les textes
    int positionTexteX = (largeurFenetre - surfaceTour->w - surfaceJoueur->w) / 2;
    int positionTexteY = posYPlateau / 2;

    int posTourX = positionTexteX;
    int posTourY = positionTexteY;

    int posJoueurX = positionTexteX + surfaceTour->w;
    int posJoueurY = positionTexteY;

    // Affichage des textes 
    SDL_Rect positionTour = {posTourX, posTourY, surfaceTour->w, surfaceTour->h};
    SDL_RenderCopy(renderer, textureTour, NULL, &positionTour);

    SDL_Rect positionJoueur = {posJoueurX, posJoueurY, surfaceJoueur->w, surfaceJoueur->h};
    SDL_RenderCopy(renderer, textureJoueur, NULL, &positionJoueur);

    // Nettoyage des surfaces et textures
    SDL_FreeSurface(surfaceTour);
    SDL_FreeSurface(surfaceJoueur);
    SDL_DestroyTexture(textureTour);
    SDL_DestroyTexture(textureJoueur);
}

/**
 * \fn afficher_coups_possibles(SDL_Renderer *renderer, t_case jeu[N][N], t_case couleur, int tailleCase, int posX, int posY)
 * \brief Affiche les coups possibles avec des cerlces non pleins
 * \param renderer Rendu SDL
 * \param jeu Matrice du jeu
 * \param couleur Couleur du joueur actuel
 * \param tailleCase Taille d'une case en pixels
 * \param posX Position X du plateau
 * \param posY Position Y du plateau
 */

 void afficher_coups_possibles(SDL_Renderer *renderer, t_case jeu[N][N], t_case couleur, int tailleCase, int posX, int posY){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int rayon = tailleCase / 3;
            int centreX = j * tailleCase + tailleCase / 2;
            int centreY = i * tailleCase + tailleCase / 2;
            
            if (jeu[i][j] == vide && coup_valide(jeu, i, j, couleur)) {
                // Dessiner la bordure du cercle
                for (int angle = 0; angle < 360; angle++) {
                    // Convertir l'angle en radians
                    float radian = angle * (3.1415 / 180.0);
                    
                    // Calculer la position du point sur le cercle à un rayon donné
                    int x = (centreX + rayon * cos(radian))+posX;
                    int y = (centreY + rayon * sin(radian))+posY;
                    
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}
    

/**
* \fn afficher_bouton(SDL_Renderer *renderer, TTF_Font *font, bouton *btn)
* \brief Affiche un bouton
* \param renderer Le rendu SDL
* \param font Police d'écriture
* \param btn Structure du bouton à afficher
*/
void afficher_bouton(SDL_Renderer *renderer, TTF_Font *font, bouton *btn) {
    // Dessiner un rectangle plein
    SDL_SetRenderDrawColor(renderer, btn->color.r, btn->color.g, btn->color.b, 255);
    SDL_RenderFillRect(renderer, &btn->rect);

    // Afficher le texte du bouton
    SDL_Color textColor;
    if(themeMode == 0) textColor = (SDL_Color){240, 229, 231, 255}; // Blanc
    else textColor = (SDL_Color){203,203,203, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, btn->text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Positionner le texte
    SDL_Rect textRect = {
        btn->rect.x + (btn->rect.w - textSurface->w) / 2,
        btn->rect.y + (btn->rect.h - textSurface->h) / 2,
        textSurface->w, textSurface->h
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Nettoyage surface et texture
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/**
* \fn clicBouton(bouton *btn, int mouseX, int mouseY)
* \brief Vérifie si un clic est dans un bouton
* \param btn Bouton à vérifier
* \param mouseX Position X du clic
* \param mouseY Position Y du clic
* \return 1 si le clic est dans le bouton, 0 sinon
*/
int clicBouton(bouton *btn, int mouseX, int mouseY) {
    return (mouseX >= btn->rect.x && mouseX <= (btn->rect.x + btn->rect.w) &&
            mouseY >= btn->rect.y && mouseY <= (btn->rect.y + btn->rect.h));
}

/**
* \fn afficherFenetreFinDePartie(SDL_Renderer* renderer, int scoreNoir, int scoreBlanc, int largeurFenetre, int hauteurFenetre, int *temps_noir, int *temps_blanc, t_case joueurActuel)
* \brief Affiche la fenêtre de fin de partie
* \param renderer Le rendu SDL
* \param scoreNoir Score du joueur noir
* \param scoreBlanc Score du joueur blanc
* \param largeurFenetre Largeur de la fenêtre
* \param hauteurFenetre Hauteur de la fenêtre
* \param temps_noir Pointeur vers le temps du joueur noir
* \param temps_blanc Pointeur vers le temps du joueur blanc
* \param joueurActuel Joueur dont c'était le tour
* \return Choix de l'utilisateur (1=rejouer, 2=menu)
*/
int afficherFenetreFinDePartie(SDL_Renderer* renderer, int scoreNoir, int scoreBlanc, int largeurFenetre, int hauteurFenetre, int *temps_noir, int *temps_blanc, t_case joueurActuel) {
    SDL_Color couleurTexte = {255, 255, 255, 255}, couleurFond;

    if(themeMode == 0) couleurFond = (SDL_Color){139, 147, 84, 255};
    else couleurFond = (SDL_Color){76,98,66, 255};

    int quitterFenetre = 0;
    int choix = 0; // 0 = aucun choix, 1 = Rejouer, 2 = Menu

    // Chargement image, texture et police d'écriture
    SDL_Surface* image = SDL_LoadBMP("../assets/blob.bmp");
    if (!image) {
        fprintf(stderr, "Erreur chargement image : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image); 
    if (!texture) {
        fprintf(stderr, "Erreur création texture : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int tailleTexte1 = largeurFenetre/25;
    TTF_Font *titre = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", tailleTexte1);
    if (!titre) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(titre);
        return EXIT_FAILURE;
    }

    int tailleTexte2 = largeurFenetre/60;
    TTF_Font *font = TTF_OpenFont("../assets/Montserrat-Light.ttf", tailleTexte2);
    if (!font) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
        return EXIT_FAILURE;
    }

    int imgLargeur, imgHauteur;
    SDL_QueryTexture(texture, NULL, NULL, &imgLargeur, &imgHauteur);
    imgLargeur = largeurFenetre/ 1.04;
    imgHauteur = largeurFenetre/ 1.04;

    // Taille des boutons 
    int boutonLargeur = largeurFenetre/4.2;
    int boutonHauteur = hauteurFenetre /11.6;

    // Position des boutons
    int posXBtnMenu = largeurFenetre * 0.20;
    int posYBtnMenu = hauteurFenetre * 0.5;
    int posXBtnRejouer = largeurFenetre * 0.5;
    int posYBtnRejouer = hauteurFenetre * 0.5;

    bouton btnRejouer = {{posXBtnRejouer, posYBtnRejouer, boutonLargeur, boutonHauteur}, couleurFond, "Rejouer"};
    bouton btnMenu = {{posXBtnMenu, posYBtnMenu, boutonLargeur, boutonHauteur}, couleurFond, "Menu"};

    while (!quitterFenetre) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { // Fermeture de la fenêtre
                quitterFenetre = 1;
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) { // Redimensionnement de la fenêtre de jeu
                largeurFenetre = e.window.data1;
                hauteurFenetre = e.window.data2;

                imgLargeur = largeurFenetre/ 1.04;
                imgHauteur = largeurFenetre/ 1.04;
            
                boutonLargeur = largeurFenetre/4.2;
                boutonHauteur = hauteurFenetre /11.6;

                posXBtnMenu = largeurFenetre * 0.20;
                posYBtnMenu = hauteurFenetre * 0.5;
                posXBtnRejouer = largeurFenetre * 0.5;
                posYBtnRejouer = hauteurFenetre * 0.5;

                btnRejouer.rect.x = posXBtnRejouer;
                btnRejouer.rect.y = posYBtnRejouer;
                btnRejouer.rect.w = boutonLargeur;
                btnRejouer.rect.h = boutonHauteur;

                btnMenu.rect.x = posXBtnMenu;
                btnMenu.rect.y = posYBtnMenu;
                btnMenu.rect.w = boutonLargeur;
                btnMenu.rect.h = boutonHauteur;

                tailleTexte1 = largeurFenetre/25;
                if (titre) {
                    TTF_CloseFont(titre);
                }
                titre = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", tailleTexte1);
                if (!titre) {
                    fprintf(stderr, "Erreur chargement police après redimensionnement : %s\n", TTF_GetError());
                    return EXIT_FAILURE;
                }

                tailleTexte2 = largeurFenetre/60;
                if(font){
                    TTF_CloseFont(font);
                }
                font = TTF_OpenFont("../assets/Montserrat-Light.ttf", tailleTexte2);
                if (!font) {
                    fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
                    SDL_DestroyTexture(texture);
                    TTF_CloseFont(titre);
                    return EXIT_FAILURE;
                }

            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (clicBouton(&btnRejouer, x, y)) {
                    choix = 1; // Rejouer
                    quitterFenetre = 1;
                } else if (clicBouton(&btnMenu, x, y)) {
                    choix = 2; // Menu
                    quitterFenetre = 1;
                }
            }
        }

        if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 139,147,84,255);
        else SDL_SetRenderDrawColor(renderer, 76,98,66,255);
        SDL_RenderClear(renderer);

        // Positionner image 
        SDL_Rect destRect = {
            (largeurFenetre - imgLargeur) / 2,  
            (hauteurFenetre - imgHauteur) / 2,
            imgLargeur,
            imgHauteur
        };

        char texteFinal[100];
        sprintf(texteFinal, "FIN DE LA PARTIE");

        SDL_Surface* surfaceTexteFinal = TTF_RenderText_Solid(titre, texteFinal, couleurTexte);
        SDL_Texture* textureTexteFinal = SDL_CreateTextureFromSurface(renderer, surfaceTexteFinal);
    
        // Positionner le texte de fin de partie qui désigne le gagnant
        SDL_Rect posTexteFinal = {
            destRect.x + (destRect.w - surfaceTexteFinal->w) / 2.3,
            hauteurFenetre * 0.25,
            surfaceTexteFinal->w,
            surfaceTexteFinal->h
        };

        char texteScore[100];
        if (*temps_noir < 0) {
            sprintf(texteScore, "LE JOUEUR BLANC GAGNE AU TEMPS");
        } else if (*temps_blanc < 0) {
            sprintf(texteScore, "LE JOUEUR NOIR GAGNE AU TEMPS");
        } else if (scoreNoir > scoreBlanc) {
            sprintf(texteScore, "LE JOUEUR NOIR GAGNE LA PARTIE (%d : %d)", scoreNoir, scoreBlanc);
        } else if (scoreNoir < scoreBlanc) {
            sprintf(texteScore, "LE JOUEUR BLANC GAGNE LA PARTIE (%d : %d)", scoreBlanc, scoreNoir);
        } else {
            if (joueurActuel == noir) {
                sprintf(texteScore, "LE JOUEUR NOIR GAGNE LA PARTIE (%d : %d)", scoreNoir, scoreBlanc);
            } else {
                sprintf(texteScore, "LE JOUEUR BLANC GAGNE LA PARTIE (%d : %d)", scoreBlanc, scoreNoir);
            }
        }

        SDL_Surface* surfaceScore = TTF_RenderText_Solid(font, texteScore, couleurTexte);
        SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);

        // Positionner les scores
        SDL_Rect posScore = {
            destRect.x + (destRect.w - surfaceScore->w) / 2.3, 
            hauteurFenetre * 0.35, 
            surfaceScore->w,
            surfaceScore->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_RenderCopy(renderer, textureScore, NULL, &posScore);
        SDL_RenderCopy(renderer, textureTexteFinal, NULL, &posTexteFinal);

        SDL_DestroyTexture(textureScore);
        SDL_DestroyTexture(textureTexteFinal);

        afficher_bouton(renderer, font, &btnRejouer);
        afficher_bouton(renderer, font, &btnMenu);

        SDL_RenderPresent(renderer);

        SDL_FreeSurface(surfaceScore);
        SDL_FreeSurface(surfaceTexteFinal);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(texture);
    TTF_CloseFont(titre);
    TTF_CloseFont(font);
    return choix;
}

/**
* \fn afficherChrono(SDL_Renderer *renderer, TTF_Font *font, int temps_rest, int x, int y, int posImgX, int posImgY, int largeurFenetre) {
* \brief Affiche un chronomètre
* \param renderer Le rendu SDL
* \param font Police utilisée
* \param temps_rest Temps restant
* \param x Position X
* \param y Position Y
* \param posImgX Position X de l'image
* \param posImgY Position Y de l'image
* \param largeurFenetre Largeur de la fenêtre
*/
void afficherChrono(SDL_Renderer *renderer, TTF_Font *font, int temps_rest, int x, int y, int posImgX, int posImgY, int largeurFenetre) {
    SDL_Surface* image;

    // Chargement image et texture
    if(themeMode == 0) image = SDL_LoadBMP("../assets/chronoClair.bmp");
    else image = SDL_LoadBMP("../assets/chronoSombre.bmp");

    if (!image) {
        fprintf(stderr, "Erreur chargement image : %s\n", SDL_GetError());
    }

    SDL_Texture* textureImg = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image); 

    if (!textureImg) {
        fprintf(stderr, "Erreur création texture : %s\n", SDL_GetError());
    }
    
    SDL_Rect img = {posImgX , posImgY, largeurFenetre/40, largeurFenetre/40};

    char texte[20];
    sprintf(texte, "%02d:%02d", temps_rest / 60, temps_rest % 60);
    
    SDL_Color couleur;
    if(themeMode == 0)couleur = (SDL_Color){71, 63, 52, 255};
    else couleur = (SDL_Color){203,203,203, 255};

    if (temps_rest <= 10) couleur = (SDL_Color){180, 50, 50, 255};  // Rouge si < 10 sec

    SDL_Surface *surface = TTF_RenderText_Solid(font, texte, couleur);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    x-= surface->w / 2;
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderCopy(renderer, textureImg, NULL, &img);

    // Nettoyer surface et texture
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(textureImg);
}

/**
* \fn initialiserDimensions(DimensionsJeu* dims)
* \brief Initialise les dimensions du jeu
* \param dims Structure des dimensions à initialiser
*/
void initialiserDimensions(DimensionsJeu* dims) {
    dims->tailleCase = (dims->hauteurFenetre + dims->largeurFenetre) / 38;
    dims->taillePolice = (dims->hauteurFenetre + dims->largeurFenetre) / 71;
    
    dims->largeurPlateau = N * dims->tailleCase;
    dims->hauteurPlateau = N * dims->tailleCase;
    
    dims->posXPlateau = (dims->largeurFenetre - dims->largeurPlateau) / 2;
    dims->posYPlateau = (dims->hauteurFenetre - dims->hauteurPlateau) / 2;
    
    dims->rayonPionScoreJ = dims->largeurFenetre / 29;
    
    dims->largeurBouton = dims->largeurFenetre / 4.5;
    dims->hauteurBouton = dims->hauteurFenetre / 15;
    dims->boutonX = dims->largeurFenetre * 0.4;
    dims->boutonY = dims->hauteurFenetre * 0.9;
}

/**
* \fn initialiserSDL(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font1, TTF_Font** font2, DimensionsJeu* dims)
* \brief Initialise SDL et les ressources
* \param window Fenêtre SDL
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture 
* \param dims Dimensions du jeu
* \return 1 si succès, 0 sinon
*/
int initialiserSDL(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font1, 
    TTF_Font** font2, DimensionsJeu* dims) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Othello", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                dims->largeurFenetre, dims->hauteurFenetre, SDL_WINDOW_RESIZABLE);
    if (!*window) {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_SetWindowMinimumSize(*window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*renderer) {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 0;
    }

    *font1 = TTF_OpenFont("../assets/Montserrat-SemiBold.ttf", dims->taillePolice);
    if (!*font1) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 0;
        
    }

    *font2 = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", dims->taillePolice);
    if (!*font2) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        TTF_CloseFont(*font1);
        TTF_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

/**
* \fn nettoyerRessources(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2)
* \brief Nettoie les ressources SDL
* \param window Fenêtre SDL
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
*/
void nettoyerRessources(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    for (int i = 0; i < 3; i++) {
        if (buttons[i].textTexture) {
            SDL_DestroyTexture(buttons[i].textTexture);
            buttons[i].textTexture = NULL;
        }
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = NULL;
    }
    if (settingsTexture) {
        SDL_DestroyTexture(settingsTexture);
        settingsTexture = NULL;
    }

    if (font1) {
        TTF_CloseFont(font1);
        font1 = NULL;
    }
    if (font2) {
        TTF_CloseFont(font2);
        font2 = NULL;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if(sonPion != NULL) {
        Mix_FreeChunk(sonPion);
        sonPion = NULL;
    }
    if(menuMusique) {
        Mix_FreeMusic(menuMusique);
        menuMusique = NULL;
    }
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

/**
* \fn mettreAJourDimensions(DimensionsJeu* dims, bouton* btnSauvegarde, TTF_Font** font1, TTF_Font** font2)
* \brief Met à jour les dimensions après redimensionnement de la fenêtre
* \param dims Dimensions du jeu
* \param btnSauvegarde Bouton de sauvegarde
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
*/
void mettreAJourDimensions(DimensionsJeu* dims, bouton* btnSauvegarde, TTF_Font** font1, TTF_Font** font2) {
    initialiserDimensions(dims);
    
    btnSauvegarde->rect.x = dims->boutonX;
    btnSauvegarde->rect.y = dims->boutonY;
    btnSauvegarde->rect.w = dims->largeurBouton;
    btnSauvegarde->rect.h = dims->hauteurBouton;
    
    *font1 = TTF_OpenFont("../assets/Montserrat-SemiBold.ttf", dims->taillePolice);
    if (!*font1) fprintf(stderr, "Erreur police 1 : %s\n", TTF_GetError());

    *font2 = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", dims->taillePolice);
    if (!*font2) fprintf(stderr, "Erreur police 2 : %s\n", TTF_GetError());
}

/**
* \fn creerMessageSauvegarde(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, SDL_Texture** texture, SDL_Rect* rect, int largeurFenetre, int hauteurFenetre)
* \brief Créer un message indiquant que la liste de sauvegarde est pleine
* \param renderer Rendu SDL
* \param font police d'écriture
* \param color Couleur du texte
* \param texture Pointeur vers la texture du message
* \param rect Rectangle définissant la position et taille du message
* \param largeurFenetre Largeur de la fenêtre de jeu
* \param hauteurFenetre Hauteur de la fenêtre de jeu
*/
void creerMessageSauvegarde(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, 
    SDL_Texture** texture, SDL_Rect* rect, int largeurFenetre, int hauteurFenetre) {
    if (*texture) {
    SDL_DestroyTexture(*texture);
    }

    SDL_Surface* msgSurface = TTF_RenderText_Solid(font, "Limite de sauvegardes atteinte", color);
    *texture = SDL_CreateTextureFromSurface(renderer, msgSurface);
    rect->w = msgSurface->w;
    rect->h = msgSurface->h;
    rect->x = largeurFenetre * 0.64;
    rect->y = hauteurFenetre * 0.91;
    SDL_FreeSurface(msgSurface);
}

/**
* \fn jouerPartie(int modeOrdi, int partieChargee, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char* fichier)
* \brief Lance une partie
* \param modeOrdi Mode IA activé (0 ou 1)
* \param partieChargee Partie qui a été sauvegardée (0 ou 1)
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
* \param difficulteIA Difficulté de l'IA (facile, moyen, difficile)
* \param coul Couleur du joueur humain (blanc ou noir)
* \param fichier Nom du fichier de sauvegarde
* \return 1 = succès
*/
int jouerPartie(int modeOrdi, int partieChargee, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char* fichier) {
    SDL_SetWindowResizable(window, SDL_TRUE); // La fenêtre peut être redimensionée
    SDL_ShowWindow(window);

    SDL_Texture* msgSauvPleinTexture = NULL;
    SDL_Rect msgSauvPleinRect = {0};
    
    SDL_Color msgCouleur;
    if(themeMode == 0) msgCouleur  = (SDL_Color){71, 63, 52, 255};
    else msgCouleur = (SDL_Color){42, 45, 35, 255};

    DimensionsJeu dims = {
        .largeurFenetre = LARGEUR_MIN_FENETRE, 
        .hauteurFenetre = HAUTEUR_MIN_FENETRE
    };
    initialiserDimensions(&dims);

    SDL_Color couleurBouton;
    if(themeMode == 0) couleurBouton = (SDL_Color){71, 63, 52, 255};
    else couleurBouton = (SDL_Color){42,45,35, 255};

    bouton btnSauvegardeQuitter = { {dims.boutonX, dims.boutonY, dims.largeurBouton, dims.hauteurBouton}, couleurBouton, "Sauvegarder"};    
    t_case jeu[N][N];
    t_case joueurActuel = noir;
    
    SDL_Event e;
    int quitter = 0;
    int temps_noir = TEMPS_LIMITE;
    int temps_blanc = TEMPS_LIMITE;
    time_t dernier_tour = time(NULL);

    // Vérifier si c'est une partie sauvegardée ou une nouvelle partie
    if (partieChargee) {
        chargerJeu(jeu, &joueurActuel, difficulteIA, fichier, &temps_noir, &temps_blanc);
    } else {
        init_jeu(jeu);
    }
    
    while (!quitter) {
        // Vérifier si le nombre max de sauvegardes est atteint
        FILE* fichier = fopen(FICHIER_SAUVEGARDE, "r");
        int nbSauvegardes = 0;
        if (fichier != NULL) {
            char ligne[MAX_NOM_FICHIER];
            while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
                nbSauvegardes++;
            }
            fclose(fichier);
        }
        
         // Change le texte du bouton si la liste des fichiers de sauvegarde est pleine
        if (nbSauvegardes >= MAX_SAUVEGARDE) {
            strcpy(btnSauvegardeQuitter.text, "Quitter");
        } else {
            strcpy(btnSauvegardeQuitter.text, "Sauvegarder");
        }
       
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { // Fermeture de la fenêtre
                SDL_SetWindowResizable(window, SDL_FALSE);
                SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture); 
                return 1;
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                int nouvelleLargeur = e.window.data1;
                int nouvelleHauteur = e.window.data2;
                
                // Limiter la largeur de la fenêtre
                if (nouvelleLargeur < LARGEUR_MIN_FENETRE) {
                    nouvelleLargeur = LARGEUR_MIN_FENETRE;
                } else if (nouvelleLargeur > LARGEUR_MAX_FENETRE) { 
                    nouvelleLargeur = LARGEUR_MAX_FENETRE;
                }
                
                dims.largeurFenetre = nouvelleLargeur;
                dims.hauteurFenetre = nouvelleHauteur;
                
                SDL_SetWindowSize(window, dims.largeurFenetre, dims.hauteurFenetre);
                
                mettreAJourDimensions(&dims, &btnSauvegardeQuitter, &font1, &font2);

                if (nbSauvegardes >= MAX_SAUVEGARDE) {
                    creerMessageSauvegarde(renderer, font1, msgCouleur, &msgSauvPleinTexture, &msgSauvPleinRect, 
                                          dims.largeurFenetre, dims.hauteurFenetre);
                }
                
            }
            
            if (!partie_terminee(jeu) && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                
                if (clicBouton(&btnSauvegardeQuitter, x, y)) {
                    if (nbSauvegardes >= MAX_SAUVEGARDE) {
                        // Bouton "Quitter"
                        SDL_SetWindowResizable(window, SDL_FALSE);
                        SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                        if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture); 
                        return 1;
                    } else {
                        // Bouton "Sauvegarder"
                        time_t maintenant = time(NULL);
                        char nomFichier[50];
                        if (modeOrdi) {
                            strftime(nomFichier, sizeof(nomFichier), "[JoueurVsIA]_%d-%m_%Hh%M.txt", localtime(&maintenant));
                        } else {
                            strftime(nomFichier, sizeof(nomFichier), "[JoueurVsJoueur]_%d-%m_%Hh%M.txt", localtime(&maintenant));
                        }
                        
                        if(difficulteIA != NULL) {
                            sauvegarderJeu(jeu, joueurActuel, *difficulteIA, nomFichier, temps_noir, temps_blanc);
                            SDL_SetWindowResizable(window, SDL_FALSE);
                            SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                            if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture); 
                            return 1;
                        }
                    }
                } else {
                    // Tour du joueur en mode hotseat
                    if (!modeOrdi || joueurActuel == coul) {
                        gererClics(&e, jeu, &joueurActuel, dims.tailleCase, dims.posXPlateau, 
                                  dims.posYPlateau, &temps_blanc, &temps_noir, &dernier_tour);
                        afficher_jeu(jeu);
                    }
                }
            }
        }

        // Mettre à jour les chronomètres
        int temps_ecoule = (int)(time(NULL) - dernier_tour);
        if (joueurActuel == noir) {
            temps_noir -= temps_ecoule;
        } else {
            temps_blanc -= temps_ecoule;
        }
        dernier_tour = time(NULL);
        
        // Tour de l'ordinateur en mode IA
        if (modeOrdi && joueurActuel == changerJoueur(coul)) {
            if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 139, 147, 84, 255);
            else SDL_SetRenderDrawColor(renderer, 76,98,66, 255);

            SDL_RenderClear(renderer);
            afficher_bouton(renderer, font1, &btnSauvegardeQuitter);
            affichageTourJoueur(renderer, font1, dims.largeurFenetre, dims.posYPlateau, joueurActuel);
            afficherPlateau(renderer, jeu, dims.tailleCase, dims.posXPlateau, dims.posYPlateau);
            afficherScore(renderer, font2, font1, score(jeu, noir), score(jeu, blanc), 
                         dims.hauteurFenetre, dims.largeurFenetre, dims.rayonPionScoreJ);

            // Vérifie si la liste de sauvegardes est pleine
            if (nbSauvegardes >= MAX_SAUVEGARDE) {
                if(msgSauvPleinTexture)SDL_DestroyTexture(msgSauvPleinTexture);
                if (!msgSauvPleinTexture) {
                    creerMessageSauvegarde(renderer, font1, msgCouleur, &msgSauvPleinTexture, &msgSauvPleinRect, 
                                         dims.largeurFenetre, dims.hauteurFenetre);
                }
                SDL_RenderCopy(renderer, msgSauvPleinTexture, NULL, &msgSauvPleinRect);
            }

            SDL_RenderPresent(renderer);
            
            SDL_Delay(1000); // Pause de 1 sec pour simuler la réflexion de l'IA 
            
            // L'IA joue le coup
            int x = dims.posXPlateau / dims.tailleCase, y = dims.posYPlateau / dims.tailleCase;
            minMaxChoix(jeu, joueurActuel, &x, &y, *difficulteIA);
            if (x != -1 && y != -1) {
                jeu[x][y] = joueurActuel;
                retourner_pions(jeu, x, y, joueurActuel);
            }

            
            joueurActuel = coul;

            if (!peut_jouer(jeu, joueurActuel)) {
                joueurActuel = (joueurActuel == noir) ? blanc : noir;
            }

            afficher_jeu(jeu);
        }
        
        // Vérifier si le temps est écoulé (en mode hotseat)
        if ((temps_noir <= 0 || temps_blanc <= 0) && !modeOrdi) {
            int choix = afficherFenetreFinDePartie(renderer, score(jeu, noir), score(jeu, blanc), 
                                                  dims.largeurFenetre, dims.hauteurFenetre, 
                                                  &temps_noir, &temps_blanc, joueurActuel);
            if (choix == 1) {
                init_jeu(jeu);
                joueurActuel = coul;
                temps_noir = TEMPS_LIMITE;
                temps_blanc = TEMPS_LIMITE;            
            } else {
                SDL_SetWindowResizable(window, SDL_FALSE);
                SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture);
                return 1;
            }
        }
        
        // Affichage
        if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 139, 147, 84, 255);
        else SDL_SetRenderDrawColor(renderer, 76,98,66, 255);
        SDL_RenderClear(renderer);

         // Vérifie si la liste de sauvegardes est pleine
        if (nbSauvegardes >= MAX_SAUVEGARDE) {
            if (!msgSauvPleinTexture) {
                creerMessageSauvegarde(renderer, font1, msgCouleur, &msgSauvPleinTexture, &msgSauvPleinRect, 
                                      dims.largeurFenetre, dims.hauteurFenetre);
            }
            SDL_RenderCopy(renderer, msgSauvPleinTexture, NULL, &msgSauvPleinRect);
        }

        afficher_bouton(renderer, font1, &btnSauvegardeQuitter);
        affichageTourJoueur(renderer, font1, dims.largeurFenetre, dims.posYPlateau, joueurActuel);
        afficherPlateau(renderer, jeu, dims.tailleCase, dims.posXPlateau, dims.posYPlateau);
        afficherScore(renderer, font2, font1, score(jeu, noir), score(jeu, blanc), 
                     dims.hauteurFenetre, dims.largeurFenetre, dims.rayonPionScoreJ);
        
        // Affiche les coups possibles pour le joueur humain
        if (!modeOrdi || joueurActuel == coul) {
            afficher_coups_possibles(renderer, jeu, joueurActuel, dims.tailleCase, 
                                    dims.posXPlateau, dims.posYPlateau);
        }
        
        // Affichage des chronos (mode hotseat)
        if (!modeOrdi) {
            afficherChrono(renderer, font1, temps_noir, dims.largeurFenetre*0.125, 
                          dims.hauteurFenetre*0.6, dims.largeurFenetre*0.065, 
                          dims.hauteurFenetre*0.59, dims.largeurFenetre);
            afficherChrono(renderer, font1, temps_blanc, dims.largeurFenetre*0.875, 
                          dims.hauteurFenetre*0.6, dims.largeurFenetre*0.815, 
                          dims.hauteurFenetre*0.59, dims.largeurFenetre);
        }
        
        if (partie_terminee(jeu)) {
            int choix = afficherFenetreFinDePartie(renderer, score(jeu, noir), score(jeu, blanc), 
                                                  dims.largeurFenetre, dims.hauteurFenetre, 
                                                  &temps_noir, &temps_blanc, joueurActuel);
            if (choix == 1) {
                init_jeu(jeu);
                joueurActuel = coul;
                temps_noir = TEMPS_LIMITE;
                temps_blanc = TEMPS_LIMITE;
            } else {
                SDL_SetWindowResizable(window, SDL_FALSE);
                SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture);
                return 1;
            }
            
        }
        SDL_RenderPresent(renderer);
    }
    if(difficulteIA) free(difficulteIA);
    if (msgSauvPleinTexture) SDL_DestroyTexture(msgSauvPleinTexture);
    return 1;
}

/**
* \fn JoueurVsOrdi(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul)
* \brief Mode Joueur vs Ordinateur (nouvelle partie)
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
* \param difficulteIA Difficulté IA
* \param coul Couleur du joueur humain
* \return 1 = succès 
*/
int JoueurVsOrdi(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul) {
    if(jouerMusique) { 
        Mix_HaltMusic();
        jouerMusique = 0; 
    }
    return jouerPartie(1, 0, renderer, font1, font2, difficulteIA, coul, ""); // 1 = mode ordinateur, 0 = nouvelle partie
}

/**
* \fn JoueurVsJoueur(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2)
* \brief Mode Joueur vs Joueur (nouvelle partie)
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
* \return 1 = succès
*/
int JoueurVsJoueur(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    if(jouerMusique) {
        Mix_HaltMusic();
        jouerMusique = 0;
    }
    t_niveau * mode = malloc(sizeof(t_niveau));
    *mode = UNK;
    return jouerPartie(0, 0, renderer, font1, font2, mode, noir, ""); // 0 = mode joueur vs joueur, 0 = nouvelle partie
}

/**
* \fn JoueurVsOrdiSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char* fichier)
* \brief Mode Joueur vs Ordinateur (partie sauvegardée)
* \param renderer Rendu SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
* \param difficulteIA Difficulté IA
* \param coul Couleur du joueur humain
* \param fichier Fichier de sauvegarde
* \return 1 = succès
*/
int JoueurVsOrdiSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_niveau * difficulteIA, t_case coul, char* fichier) {
    if(jouerMusique) {
        Mix_HaltMusic();
        jouerMusique = 0;
    }
    return jouerPartie(1, 1, renderer, font1, font2, difficulteIA, coul, fichier); // 1 = mode ordinateur, 1 = partie sauvegardée
}

/**
* \fn JoueurVsJoueurSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, char* fichier);
* \brief Mode Joueur vs Joueur (partie sauvegardée)
* \param renderer Renderer SDL
* \param font1 Première police d'écriture
* \param font2 Deuxième police d'écriture
* \param fichier Fichier de sauvegarde
* \return 1 = succès
*/
int JoueurVsJoueurSauv(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, char* fichier) {
    if(jouerMusique) {
        Mix_HaltMusic();
        jouerMusique = 0;
    }
    t_niveau * mode = malloc(sizeof(t_niveau));
    *mode = UNK;
    return jouerPartie(0, 1, renderer, font1, font2, mode, noir, fichier); // 0 = mode joueur vs joueur, 1 = partie sauvegardée
}

/**
* \brief Affiche la fenêtre de fin de partie pour le jeu en ligne
* \param renderer Le rendu SDL
* \param scoreNoir Score du joueur noir
* \param scoreBlanc Score du joueur blanc
* \param largeurFenetre Largeur de la fenêtre
* \param hauteurFenetre Hauteur de la fenêtre
* \param temps_noir Pointeur vers le temps du joueur noir
* \param temps_blanc Pointeur vers le temps du joueur blanc
* \param joueurActuel Joueur dont c'était le tour
* \return Choix de l'utilisateur (2=menu)
*/
int afficherFenetreFinDePartie_bis(SDL_Renderer* renderer, int scoreNoir, int scoreBlanc, int largeurFenetre, int hauteurFenetre, int *temps_noir, int *temps_blanc, t_case joueurActuel) {
    SDL_Color couleurTexte = {255, 255, 255, 255}, couleurFond;

    if(themeMode == 0) couleurFond = (SDL_Color){139, 147, 84, 255};
    else couleurFond = (SDL_Color){76,98,66, 255};

    int quitterFenetre = 0;
    int choix = 0; // 0 = aucun choix, 1 = Rejouer, 2 = Menu

    // Chargement image, texture et police d'écriture
    SDL_Surface* image = SDL_LoadBMP("../assets/blob.bmp");
    if (!image) {
        fprintf(stderr, "Erreur chargement image : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image); 
    if (!texture) {
        fprintf(stderr, "Erreur création texture : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int tailleTexte1 = largeurFenetre/25;
    TTF_Font *titre = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", tailleTexte1);
    if (!titre) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(titre);
        return EXIT_FAILURE;
    }

    int tailleTexte2 = largeurFenetre/60;
    TTF_Font *font = TTF_OpenFont("../assets/Montserrat-Light.ttf", tailleTexte2);
    if (!font) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
        return EXIT_FAILURE;
    }

    int imgLargeur, imgHauteur;
    SDL_QueryTexture(texture, NULL, NULL, &imgLargeur, &imgHauteur);
    imgLargeur = largeurFenetre/ 1.04;
    imgHauteur = largeurFenetre/ 1.04;

    // Taille des boutons 
    int boutonLargeur = largeurFenetre/4.2;
    int boutonHauteur = hauteurFenetre /11.6;

    // Position des boutons
    int posXBtnMenu = largeurFenetre * 0.35;
    int posYBtnMenu = hauteurFenetre * 0.5;

    bouton btnMenu = {{posXBtnMenu, posYBtnMenu, boutonLargeur, boutonHauteur}, couleurFond, "Menu"};

    while (!quitterFenetre) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { // Fermeture de la fenêtre
                quitterFenetre = 1;
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) { // Redimensionnement de la fenêtre de jeu
                largeurFenetre = e.window.data1;
                hauteurFenetre = e.window.data2;

                imgLargeur = largeurFenetre/ 1.04;
                imgHauteur = largeurFenetre/ 1.04;
            
                boutonLargeur = largeurFenetre/4.2;
                boutonHauteur = hauteurFenetre /11.6;

                posXBtnMenu = largeurFenetre * 0.20;
                posYBtnMenu = hauteurFenetre * 0.5;
              

                btnMenu.rect.x = posXBtnMenu;
                btnMenu.rect.y = posYBtnMenu;
                btnMenu.rect.w = boutonLargeur;
                btnMenu.rect.h = boutonHauteur;

                tailleTexte1 = largeurFenetre/25;
                if (titre) {
                    TTF_CloseFont(titre);
                }
                titre = TTF_OpenFont("../assets/ArchivoBlack-Regular.ttf", tailleTexte1);
                if (!titre) {
                    fprintf(stderr, "Erreur chargement police après redimensionnement : %s\n", TTF_GetError());
                    return EXIT_FAILURE;
                }

                tailleTexte2 = largeurFenetre/60;
                if(font){
                    TTF_CloseFont(font);
                }
                font = TTF_OpenFont("../assets/Montserrat-Light.ttf", tailleTexte2);
                if (!font) {
                    fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
                    SDL_DestroyTexture(texture);
                    TTF_CloseFont(titre);
                    return EXIT_FAILURE;
                }

            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (clicBouton(&btnMenu, x, y)) {
                    choix = 2; // Menu
                    quitterFenetre = 1;
                }
            }
        }

        if(themeMode == 0) SDL_SetRenderDrawColor(renderer, 139,147,84,255);
        else SDL_SetRenderDrawColor(renderer, 76,98,66,255);
        SDL_RenderClear(renderer);

        // Positionner image 
        SDL_Rect destRect = {
            (largeurFenetre - imgLargeur) / 2,  
            (hauteurFenetre - imgHauteur) / 2,
            imgLargeur,
            imgHauteur
        };

        char texteFinal[100];
        sprintf(texteFinal, "FIN DE LA PARTIE");

        SDL_Surface* surfaceTexteFinal = TTF_RenderText_Solid(titre, texteFinal, couleurTexte);
        SDL_Texture* textureTexteFinal = SDL_CreateTextureFromSurface(renderer, surfaceTexteFinal);
    
        // Positionner le texte de fin de partie qui désigne le gagnant
        SDL_Rect posTexteFinal = {
            destRect.x + (destRect.w - surfaceTexteFinal->w) / 2.3,
            hauteurFenetre * 0.25,
            surfaceTexteFinal->w,
            surfaceTexteFinal->h
        };

        char texteScore[100];
        if (*temps_noir < 0) {
            sprintf(texteScore, "LE JOUEUR BLANC GAGNE AU TEMPS");
        } else if (*temps_blanc < 0) {
            sprintf(texteScore, "LE JOUEUR NOIR GAGNE AU TEMPS");
        } else if (scoreNoir > scoreBlanc) {
            sprintf(texteScore, "LE JOUEUR NOIR GAGNE LA PARTIE (%d : %d)", scoreNoir, scoreBlanc);
        } else if (scoreNoir < scoreBlanc) {
            sprintf(texteScore, "LE JOUEUR BLANC GAGNE LA PARTIE (%d : %d)", scoreBlanc, scoreNoir);
        } else {
            if (joueurActuel == noir) {
                sprintf(texteScore, "LE JOUEUR NOIR GAGNE LA PARTIE (%d : %d)", scoreNoir, scoreBlanc);
            } else {
                sprintf(texteScore, "LE JOUEUR BLANC GAGNE LA PARTIE (%d : %d)", scoreBlanc, scoreNoir);
            }
        }

        SDL_Surface* surfaceScore = TTF_RenderText_Solid(font, texteScore, couleurTexte);
        SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);

        // Positionner les scores
        SDL_Rect posScore = {
            destRect.x + (destRect.w - surfaceScore->w) / 2.3, 
            hauteurFenetre * 0.35, 
            surfaceScore->w,
            surfaceScore->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_RenderCopy(renderer, textureScore, NULL, &posScore);
        SDL_RenderCopy(renderer, textureTexteFinal, NULL, &posTexteFinal);

        SDL_DestroyTexture(textureScore);
        SDL_DestroyTexture(textureTexteFinal);
        afficher_bouton(renderer, font, &btnMenu);

        SDL_RenderPresent(renderer);

        SDL_FreeSurface(surfaceScore);
        SDL_FreeSurface(surfaceTexteFinal);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(texture);
    TTF_CloseFont(titre);
    TTF_CloseFont(font);
    return choix;
}
