#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

/* ================================================================
 * CONSTANTES
 * ================================================================ */

#define GRID_W           300
#define GRID_H           300
#define NB_AGENTS        20000
#define NB_INFECTES_INIT 20
#define NB_JOURS         365
#define SEED             42

#define MOYENNE_dE       3.0
#define MOYENNE_dI       7.0
#define MOYENNE_dR       365.0
#define FORCE_INFECTION  0.5
#define MAX_AGENTS_LOCAL (NB_AGENTS * 3)

/* ================================================================
 * ETATS
 * ================================================================ */

typedef enum { S = 0, E = 1, I = 2, R = 3 } Statut;

/* ================================================================
 * STRUCTURE INDIVIDU
 * ================================================================ */

typedef struct {
    int    x, y;
    Statut statut;
    int    timer;
    int    dE, dI, dR;
} Individu;

/* ================================================================
 * VARIABLES GLOBALES MPI
 * ================================================================ */

int rank;        /* rang du processus courant  */
int nb_procs;    /* nombre total de processus  */

/* Bande de lignes geree par ce processus */
int ligne_debut; /* premiere ligne de la bande (incluse) */
int ligne_fin;   /* derniere ligne de la bande (incluse)  */
int nb_lignes;   /* nombre de lignes dans la bande        */

/* Voisins pour l'echange des lignes fantomes */
int voisin_haut;
int voisin_bas;



/* Allocation dynamique car nb_lignes depend du nombre de processus */
int (*grille_I_locale)[GRID_W] = NULL;

/* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */

static inline double rand01(void) {
    return rand() / (double)RAND_MAX;
}

static inline int rand_int(int A, int B) {
    return A + rand() % (B - A + 1);
}

static inline double neg_exp(double moyenne) {
    double u = rand01();
    while (u == 1.0) u = rand01();
    return -moyenne * log(1.0 - u);
}

/* ================================================================
 * Bandes locales
 * ================================================================ */
 void bandes_locales(void){
 //nombre de lignes est divisible par les procs on fait un scatter
 if(GRID_H % nb_procs == 0){
    nb_lignes = GRID_H / nb_procs;
    ligne_debut = rank * nb_lignes;
    ligne_fin = ligne_debut + nb_lignes - 1;
    //MPI_Scatter() pour scatter la grille globale en bandes locales
    MPI_Scatter(grille_I_locale, nb_lignes * GRID_W, MPI_INT, grille_I_locale, nb_lignes * GRID_W, MPI_INT, 0, MPI_COMM_WORLD);
 }
 else{
 // cas ou il y a des restes
    int reste = GRID_H % nb_procs;
    if(rank < reste){
        nb_lignes = GRID_H / nb_procs + 1;
        ligne_debut = rank * nb_lignes;
        ligne_fin = ligne_debut + nb_lignes - 1;
    }
    else{
        nb_lignes = GRID_H / nb_procs;
        ligne_debut = rank * nb_lignes + reste;
        ligne_fin = ligne_debut + nb_lignes - 1;
    }
     //MPI_Scatterv() pour scatter la grille globale en bandes locales de tailles differentes
    int *sendcounts = malloc(nb_procs * sizeof(int));
    int *displs = malloc(nb_procs * sizeof(int));
    for(int i = 0; i < nb_procs; i++){
        if(i < reste){
            sendcounts[i] = (GRID_H / nb_procs + 1) * GRID_W;
            displs[i] = i * (GRID_H / nb_procs + 1) * GRID_W;
        }
        else{
            sendcounts[i] = (GRID_H / nb_procs) * GRID_W;
            displs[i] = (i * (GRID_H / nb_procs) + reste) * GRID_W;
        }
    }
    MPI_Scatterv(grille_I_locale, sendcounts, displs, MPI_INT, grille_I_locale, nb_lignes * GRID_W, MPI_INT, 0, MPI_COMM_WORLD);
     free(sendcounts);
     free(displs);
 }
 
 }
 
 
 /* ================================================================
 * Initialisation et distribution des agents
 * ================================================================ */
void initialisation_et_distribution_agents(Individu *agents) {
    for (int i = 0; i < NB_AGENTS; i++) {
        agents[i].x = rand_int(0, GRID_W - 1);
        agents[i].y = rand_int(0, GRID_H - 1);
        agents[i].statut = S;
        agents[i].timer = 0;
        agents[i].dE = (int)neg_exp(MOYENNE_dE);
        agents[i].dI = (int)neg_exp(MOYENNE_dI);
        agents[i].dR = (int)neg_exp(MOYENNE_dR);
    }
    // Infecter les individus au hasard
    for (int k = 0; k < NB_INFECTES_INIT; k++) {
            int index;
            do {
                index = rand_int(0, NB_AGENTS - 1);
            } while (tous[index].statut != S);
            tous[index].statut = I;
        }
   
   
   //nombre d'agents a send a chaque processus
   for(int p =0; p < nb_procs; p++){
        nbre_agents_local = 0;
        for(int i = 0; i < NB_AGENTS; i++){
            if(agents[i].y >= ligne_debut && agents[i].y <= ligne_fin){
                nbre_agents_local++;
            }
        }
   }
   }
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
 
 /* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */
