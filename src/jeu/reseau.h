#ifndef RESEAU_H
#define RESEAU_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>


#define PORT_SERVEUR 30000
#define TAILLE_BUFFER 512
#define ARRET "quit"

/**
* \file reseau.h
* \brief Fonctions et structures de données nécessaires à la partie du reséau.
* \author Chaosok Kong(responsable), Nasreddine Biya, Aly Rida Mahjoub 
*/
typedef struct 
{
    int server_socket;
    int client_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct hostent *serveur_info;
}contexte_reseau_t ;
//paquet de données à envoyer
typedef struct {
    t_case grille[N][N];
    t_case joueur_actif;
} paquet_jeu;
/* Déclarations de fonctions */
void fermer_serveur_socket(contexte_reseau_t* ctx);
int gererClics_reseau(SDL_Event *e, t_case jeu[N][N], t_case *joueurActuel, int tailleCase, int positionPlateauX, int positionPlateauY, int *temps_noir, int *temps_blanc, time_t *dernier_tour);
void fermer_client_socket(contexte_reseau_t* ctx);
int installer_serveur(contexte_reseau_t* ctx);
int accepter_client(contexte_reseau_t* ctx);
void serveur_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);
int connecter_server(contexte_reseau_t* ctx);
void client_joue_jeu(contexte_reseau_t* ctx, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font1, TTF_Font* font2);

#endif