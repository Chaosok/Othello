#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "fonc_jeu.h"
#include "fonc_sauv.h"
#include "../IA/fonc_ia.h"
#include "reseau.h"
#include "../graphique/plateau.h"
#include "../graphique/menus.h"
#include <time.h>
#ifdef _WIN32
 #include <winsock2.h>
 #include <windows.h>
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h> 
#endif
/**
* \file reseau.c
* \brief Définition des fonctions pour mettre en place le jeu en ligne.
* \author Chaosok Kong(responsable), Nasreddine Biya, Aly Rida Mahjoub
*/

t_case jeu[N][N];


int temps_noir = TEMPS_LIMITE, temps_blanc = TEMPS_LIMITE;


char buffer[TAILLE_BUFFER];

/** 
* \fn fermer_serveur_socket(contexte_reseau_t* ctx)
* \brief Fermer la socket du côté de serveur
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
*/
void fermer_serveur_socket(contexte_reseau_t* ctx) {
 if (ctx->server_socket != -1) {
    shutdown(ctx->server_socket, SHUT_RDWR);
    close(ctx->server_socket);
    ctx->server_socket = -1;
 }
}
/**
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
int gererClics_reseau(SDL_Event *e, t_case jeu[N][N], t_case *joueurActuel, int tailleCase, int positionPlateauX, int positionPlateauY, int *temps_noir, int *temps_blanc, time_t *dernier_tour) {
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
            int temps_ecoule = (int)(time(NULL) - *dernier_tour); 
            if (*joueurActuel == noir) {
                *temps_noir -= temps_ecoule;
            } else {
                *temps_blanc -= temps_ecoule;
            }
            *joueurActuel = (*joueurActuel == noir) ? blanc : noir;
            *dernier_tour = time(NULL);

            return 1;
        }
        
     return 0;
    }
    return 0;
}

/** 
* \fn fermer_client_socket(contexte_reseau_t* ctx)
* \brief Fermer la socket du côté de client.
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
*/
void fermer_client_socket(contexte_reseau_t* ctx) {
 if (ctx->server_socket != -1) {
 shutdown(ctx->client_socket, SHUT_RDWR);
 close(ctx->client_socket);
 ctx->client_socket = -1;
 }
}

/** 
* \fn installer_serveur(contexte_reseau_t* ctx)
* \brief Installer le serveur, en créeant la socket, en associant à une IP et un port de machine locale et mettre en écoute
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
* \return 0 est bien installé le serveur, ou -1 exite une erreur en cours d'installation du serveur
*/
int installer_serveur(contexte_reseau_t* ctx) {

 /* creation de socket */
 ctx->client_socket = -1;
 ctx->server_socket = -1;
 memset(&ctx->client_addr, 0, sizeof(ctx->client_addr));
 memset(&ctx->server_addr, 0, sizeof(ctx->server_addr));
 ctx->serveur_info = NULL;
 ctx->server_socket = socket(AF_INET, SOCK_STREAM, 0);
 if (ctx->server_socket == -1) {
    printf("Erreur création socket");
    return -1;

 }
 //initialiser les parametres du serveur
 memset(&ctx->server_addr, 0, sizeof(ctx->server_addr));
 ctx->server_addr.sin_family = AF_INET;
 ctx->server_addr.sin_port = htons(PORT_SERVEUR);
 ctx->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 int opt = 1;
 setsockopt(ctx->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 #ifdef SO_REUSEPORT
 setsockopt(ctx->server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
 #endif
 /* bind serveur - socket */
 if (bind(ctx->server_socket, (struct sockaddr*)&ctx->server_addr, sizeof(ctx->server_addr))) {
    perror("Erreur bind");
    return -1;
 }
 /* ecoute sur la socket */
 if (listen(ctx->server_socket, 5)) {
    printf("Erreur listen");
    return -1;
 }
 return 0;

}
/** 
* \fn accepter_client(contexte_reseau_t* ctx)
* \brief Accepter la demande de connexion de la socket du client
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
* \return 0 si l'acceptation de connexion réussit, -1 sinon
*/
/* accept la connexion */

int accepter_client(contexte_reseau_t* ctx) {
 socklen_t client_len = sizeof(ctx->client_addr);
 ctx->client_socket = accept(ctx->server_socket, 
 (struct sockaddr*)&ctx->client_addr, 
 &client_len);
 if (ctx->client_socket == -1) {
    return -1;
 }
 return 0;
}


/** 
* \fn envoyer_etat_jeu(int socket, t_case jeu[N][N], t_case joueur_actif);
* \brief Envoyer le paquet de données au socket destinataire
* \param socket socket de destinataire
* \param jeu plateau du jeu
* \param joueur_actif joueur courant
*/
void envoyer_etat_jeu(int socket, t_case jeu[N][N], t_case joueur_actif) {
 paquet_jeu paquet;
 memcpy(paquet.grille, jeu, sizeof(t_case) * N * N);
 paquet.joueur_actif = joueur_actif;
 send(socket, &paquet, sizeof(paquet_jeu), 0);

}


/** 
* \fn connecter_server(contexte_reseau_t* ctx);
* \brief Connecter la socket de client à celle de serveur
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
* \return 0 si connexion réussit, -1 sinon
*/
int connecter_server(contexte_reseau_t* ctx) {
 ctx->client_socket = socket(AF_INET, SOCK_STREAM, 0);
 if (ctx->client_socket < 0) {
    printf("Erreur création socket client");
    return -1;
 }

 long hostAddr;
 bzero(&(ctx->server_addr),sizeof(ctx->server_addr));
 hostAddr = inet_addr(ipInput);
 if ( (long)hostAddr != (long)-1 ){
    bcopy(&hostAddr,&(ctx->server_addr.sin_addr),sizeof(hostAddr));
 } else {
    ctx->serveur_info = gethostbyname(ipInput);
    if (ctx->serveur_info == NULL) {
        printf("Impossible de récupérer les infos du serveur\n");
    }

    bcopy(&(ctx->serveur_info->h_addrtype),&(ctx->server_addr.sin_addr),ctx->serveur_info->h_length);
 }
 ctx->server_addr.sin_port = htons(PORT_SERVEUR);
 ctx->server_addr.sin_family = AF_INET;
 //requete de connexion
 if (connect(ctx->client_socket, (struct sockaddr*)&ctx->server_addr, sizeof(ctx->server_addr))){
    return -1;
 }
 return 0;

}


/** 
* \fn recevoir_etat_jeu(int socket, t_case jeu[N][N], t_case joueur_actif);
* \brief Recevoir le paquet de données depuis socket expéditeur
* \param socket descripteur de socket expéditeur
* \param jeu plateau du jeu
* \param joueur_actif joueur courant
*/
int recevoir_etat_jeu(int socket, t_case jeu[N][N], t_case* joueur_actif) {
 paquet_jeu paquet;
 int result = recv(socket, &paquet, sizeof(paquet_jeu), 0);
 if (result <= 0) {
    perror("Erreur réception paquet");
    return result;
 }
 memcpy(jeu, paquet.grille, sizeof(t_case) * N * N);
 *joueur_actif = paquet.joueur_actif;
 return result;
}

/** 
* \fn socket_data_ready(int socket_fd);
* \brief Vérifie si des données sont disponibles en lecture sur une socket
* \param socket_fd Descripteur de la socket à surveiller.
* \return int Retourne 1 si des données sont prêtes à être lues, 0 sinon.
*/
int socket_data_ready(int socket_fd) {
    struct pollfd pfd = { .fd = socket_fd, .events = POLLIN };
    return poll(&pfd, 1, 0) > 0;
}

/** 
* \fn render_plateau(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_case joueur_actif, DimensionsJeu dims, int show_possible_moves);
* \brief Affiche l'ensemble de l'interface graphique du plateau de jeu
* \param renderer Pointeur vers le renderer SDL utilisé pour dessiner.
* \param font1 Police utilisée pour l'affichage du tour du joueur.
* \param font2 Police utilisée pour l'affichage des scores.
* \param joueur_actif Joueur courant
* \param dims Structure contenant les dimensions et positions utiles à l'affichage.
* \param show_possible_moves Si différent de 0, les coups possibles sont affichés sur le plateau.
*/
void render_plateau(SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2, t_case joueur_actif,
    DimensionsJeu dims, int show_possible_moves) {
    SDL_SetRenderDrawColor(renderer, themeMode ? 76 : 139, themeMode ? 98 : 147, themeMode ? 66 : 84, 255);
    SDL_RenderClear(renderer);
    affichageTourJoueur(renderer, font1, dims.largeurFenetre, dims.posYPlateau, joueur_actif);
    afficherPlateau(renderer, jeu, dims.tailleCase, dims.posXPlateau, dims.posYPlateau);
    afficherScore(renderer, font2, font1, score(jeu, noir), score(jeu, blanc),
    dims.hauteurFenetre, dims.largeurFenetre, dims.rayonPionScoreJ);
    if (show_possible_moves)
        afficher_coups_possibles(renderer, jeu, joueur_actif, dims.tailleCase, dims.posXPlateau, dims.posYPlateau);
    SDL_RenderPresent(renderer);

}


/** 
* \fn serveur_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
* \brief Le joueur du côté serveur joue la partie en suivant la logique du jeu
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
* \param window la fenêtre principale SDL de l'application.
* \param renderer Pointeur vers le renderer SDL utilisé pour dessiner.
* \param font1 Police utilisée pour l'affichage du tour du joueur.
* \param font2 Police utilisée pour l'affichage des scores.
*/
void serveur_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_ShowWindow(window);

    DimensionsJeu dims = { LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    initialiserDimensions(&dims);

    int quit = 0, monTour = 1;
    time_t dernier_tour = time(NULL);
    t_case joueur_actif = noir;

    init_jeu(jeu);
    envoyer_etat_jeu(ctx->client_socket, jeu, joueur_actif);

    while (!quit) {
        SDL_Event e;
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                send(ctx->client_socket, "Q", 1, 0);
                quit = 1;
            }

            if (monTour && joueur_actif == noir && !partie_terminee(jeu)) {
               
                if (e.type == SDL_MOUSEBUTTONDOWN && peut_jouer(jeu, joueur_actif)) {
                    int bon_mvt = gererClics_reseau(&e, jeu, &joueur_actif, dims.tailleCase,
                                             dims.posXPlateau, dims.posYPlateau,
                                             &temps_blanc, &temps_noir, &dernier_tour);
                    if (bon_mvt) {
                        
                        envoyer_etat_jeu(ctx->client_socket, jeu, joueur_actif);
                        monTour = 0;
                    }
                }
            }
        } 

        
        if (monTour && joueur_actif == noir && !partie_terminee(jeu)) {
           
            if (!peut_jouer(jeu, joueur_actif)) {
                joueur_actif = changerJoueur(joueur_actif);
                envoyer_etat_jeu(ctx->client_socket, jeu, joueur_actif);
                monTour = 0;
                
                SDL_Delay(50);
            }
        }

        
        if (!monTour && socket_data_ready(ctx->client_socket)) {
            char peek;
            
            if (recv(ctx->client_socket, &peek, 1, MSG_PEEK) > 0 && peek == 'J') {
                recv(ctx->client_socket, &peek, 1, 0);  // Consume 'J'
                init_jeu(jeu);
                continue;
            }
            if (recv(ctx->client_socket, &peek, 1, MSG_PEEK) > 0 && peek == 'Q') {
                recv(ctx->client_socket, &peek, 1, 0); // Consume 'Q'
                quit = 1;
                continue;
            }
            if (recevoir_etat_jeu(ctx->client_socket, jeu, &joueur_actif) <= 0) {
                quit = 1;
                continue;
            }
            monTour = 1;
            
            dernier_tour = time(NULL);
        }

        if (partie_terminee(jeu)) {
            temps_noir = TEMPS_LIMITE;
            temps_blanc = TEMPS_LIMITE;
            int choix = afficherFenetreFinDePartie_bis(renderer, score(jeu, noir), score(jeu, blanc), 
                                                        dims.largeurFenetre, dims.hauteurFenetre, 
                                                        &temps_noir, &temps_blanc, joueur_actif);
            if (choix == 1) {
                send(ctx->client_socket, "J", 1, 0);
                joueur_actif = noir;
                temps_noir = TEMPS_LIMITE;
                temps_blanc = TEMPS_LIMITE;
            } else {
                send(ctx->client_socket, "Q", 1, 0);
                SDL_SetWindowResizable(window, SDL_FALSE);
                SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                quit = 1;
            }
        }

        
        render_plateau(renderer, font1, font2, joueur_actif, dims, monTour && joueur_actif == noir);
    } 

    
    fermer_client_socket(ctx);
    fermer_serveur_socket(ctx);
    serveurPret = 0;
    clientAccepte = 0;
    threadLance = 0;
    ctx->client_socket = -1;
    ctx->server_socket = -1;
    memset(&ctx->client_addr, 0, sizeof(ctx->client_addr));
    memset(&ctx->server_addr, 0, sizeof(ctx->server_addr));
    ctx->serveur_info = NULL;
    currentMenu = MENU_MAIN;
}



/** 
* \fn client_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
* \brief Le joueur du côté client joue la partie en suivant la logique du jeu
* \param ctx contexte de réseau contient les informations nécessaires pour la communication.
* \param window la fenêtre principale SDL de l'application.
* \param renderer Pointeur vers le renderer SDL utilisé pour dessiner.
* \param font1 Police utilisée pour l'affichage du tour du joueur.
* \param font2 Police utilisée pour l'affichage des scores.
*/
void client_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2) {
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_ShowWindow(window);

    DimensionsJeu dims = { LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    initialiserDimensions(&dims);

    int quit = 0, monTour = 0;
    time_t dernier_tour = time(NULL);
    t_case joueur_actif;

    init_jeu(jeu);
    if (recevoir_etat_jeu(ctx->client_socket, jeu, &joueur_actif) <= 0)
        return;

    while (!quit) {
        SDL_Event e;
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                send(ctx->client_socket, "Q", 1, 0);
                quit = 1;
            }

            if (monTour && joueur_actif == blanc && !partie_terminee(jeu)) {
                
                if (e.type == SDL_MOUSEBUTTONDOWN && peut_jouer(jeu, joueur_actif)) {
                    int bon_mvt = gererClics_reseau(&e, jeu, &joueur_actif, dims.tailleCase,
                                             dims.posXPlateau, dims.posYPlateau,
                                             &temps_blanc, &temps_noir, &dernier_tour);
                    if (bon_mvt) {
                        envoyer_etat_jeu(ctx->client_socket, jeu, joueur_actif);
                        monTour = 0;
                    }
                }
            }
        } 

       
        if (monTour && joueur_actif == blanc && !partie_terminee(jeu)) {
            if (!peut_jouer(jeu, joueur_actif)) {
                
                joueur_actif = changerJoueur(joueur_actif);
                envoyer_etat_jeu(ctx->client_socket, jeu, joueur_actif);
                monTour = 0;
                SDL_Delay(50);  
            }
        }

        
        if (!monTour && socket_data_ready(ctx->client_socket)) {
            char peek;
            if (recv(ctx->client_socket, &peek, 1, MSG_PEEK) > 0 && peek == 'J') {
                recv(ctx->client_socket, &peek, 1, 0); 
                init_jeu(jeu);
                continue;
            } else if (recv(ctx->client_socket, &peek, 1, MSG_PEEK) > 0 && peek == 'Q') {
                recv(ctx->client_socket, &peek, 1, 0); 
                quit = 1;
                continue;
            }
            if (recevoir_etat_jeu(ctx->client_socket, jeu, &joueur_actif) <= 0) {
                quit = 1;
                continue;
            }
            monTour = 1;
            dernier_tour = time(NULL);
        }

        
        if (partie_terminee(jeu)) {
            int choix = afficherFenetreFinDePartie_bis(renderer, score(jeu, noir), score(jeu, blanc),
                                                        dims.largeurFenetre, dims.hauteurFenetre, 
                                                        &temps_noir, &temps_blanc, joueur_actif);
            if (choix == 1) {
                send(ctx->client_socket, "J", 1, 0);
                joueur_actif = blanc;
                temps_noir = TEMPS_LIMITE;
                temps_blanc = TEMPS_LIMITE;
            } else {
                send(ctx->client_socket, "Q", 1, 0);
                SDL_SetWindowResizable(window, SDL_FALSE);
                SDL_SetWindowSize(window, LARGEUR_MIN_FENETRE, HAUTEUR_MIN_FENETRE);
                quit = 1;
            }
            quit = 1;
        }

        
        render_plateau(renderer, font1, font2, joueur_actif, dims, monTour && joueur_actif == blanc);
    } 

   
    fermer_serveur_socket(ctx);
    fermer_client_socket(ctx);
    serveurPret = 0;
    clientAccepte = 0;
    threadLance = 0;
    ctx->client_socket = -1;
    ctx->server_socket = -1;
    memset(&ctx->client_addr, 0, sizeof(ctx->client_addr));
    memset(&ctx->server_addr, 0, sizeof(ctx->server_addr));
    ctx->serveur_info = NULL;
    currentMenu = MENU_MAIN;
}
