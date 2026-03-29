#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* ================================================================
 * CONSTANTES DE LA SIMULATION
 * ================================================================ */

#define GRID_W           300    /* largeur de la grille              */
#define GRID_H           300    /* hauteur de la grille              */
#define NB_AGENTS        20000  /* nombre total d'individus          */
#define NB_INFECTES_INIT 20     /* individus infectes au depart      */
#define NB_JOURS         365    /* duree de la simulation            */
#define SEED             42     /* graine du generateur aleatoire    */

#define MOYENNE_dE       3.0    /* duree moyenne de l'etat E (jours) */
#define MOYENNE_dI       7.0    /* duree moyenne de l'etat I (jours) */
#define MOYENNE_dR       365.0  /* duree moyenne de l'etat R (jours) */
#define FORCE_INFECTION  0.5    /* parametre beta de contamination   */

/* ================================================================
 * ETATS POSSIBLES D'UN INDIVIDU
 * ================================================================ */

typedef enum {
    S = 0,  /* Susceptible  : peut etre contamine          */
    E = 1,  /* Exposed      : contamine mais pas contagieux */
    I = 2,  /* Infected     : contagieux                   */
    R = 3   /* Recovered    : immunise temporairement       */
} Statut;

/* ================================================================
 * STRUCTURE D'UN INDIVIDU
 * ================================================================ */

typedef struct {
    int    x, y;    /* position sur la grille                       */
    Statut statut;  /* etat courant : S, E, I ou R                  */
    int    timer;   /* nb de jours passes dans l'etat courant       */
    int    dE;      /* duree personnelle de la periode E            */
    int    dI;      /* duree personnelle de la periode I            */
    int    dR;      /* duree personnelle de la periode R            */
} Individu;

/* ================================================================
 * GRILLE DE COMPTAGE DES INFECTES
 * ================================================================
 *
 * Pour chaque cellule (x,y), grille_I[y][x] contient le nombre
 * d'agents I presents. Permet de calculer le voisinage en le parcourant
 * au lieu de parcourir les 20000 agents pour chaque agent S.
 */
int grille_I[GRID_H][GRID_W];

/* ================================================================
 * FONCTIONS ALEATOIRES
 * ================================================================ */

/* Reel uniforme dans [0, 1] */
static inline double rand01(void) {
    return rand() / (double)RAND_MAX;
}

/* Entier uniforme dans [A, B] inclus */
static inline int rand_int(int A, int B) {
    return A + rand() % (B - A + 1);
}

/* Distribution exponentielle de moyenne "moyenne"
 * Methode : transformee inverse -> x = -moyenne * log(U)
 * Le while evite log(0) si rand() retourne 0 */
static inline double neg_exp(double moyenne) {
    double u = rand01();
    while (u == 1.0) u = rand01();
    return -moyenne * log(1-u);
}

/* ================================================================
 * INITIALISATION DES INDIVIDUS
 * ================================================================ */

void initialiser_agents(Individu *agents) {

    /* Tous les individus commencent en etat S */
    for (int k = 0; k < NB_AGENTS; k++) {
        Individu *a = &agents[k];

        a->x      = rand_int(0, GRID_W - 1);
        a->y      = rand_int(0, GRID_H - 1);
        a->statut = S;
        a->timer  = 0;

        /* Durees personnelles tirees une seule fois a la creation. Le +1 garantit une duree minimale de 1 jour. */
        a->dE = (int)neg_exp(MOYENNE_dE) + 1;
        a->dI = (int)neg_exp(MOYENNE_dI) + 1;
        a->dR = (int)neg_exp(MOYENNE_dR) + 1;
    }

    /* Les NB_INFECTES_INIT premiers individus sont infectes */
    for (int k = 0; k < NB_INFECTES_INIT; k++) {
        int index;
        do {
            index = rand_int(0, NB_AGENTS - 1);
        } while (agents[index].statut == I);
        Individu *b = &agents[index];
        b -> statut = I;
    }
}

/* ================================================================
 * MISE A JOUR DE LA GRILLE DE COMPTAGE
 * ================================================================ */

/* Reconstruit entierement la grille depuis zero.
 * A appeler apres chaque deplacement. */
void reconstruire_grille_I(Individu *agents) {

    /* Remise a zero */
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            grille_I[y][x] = 0;
        }
    }

    /* Comptage des infectes */
    for (int k = 0; k < NB_AGENTS; k++) {
        if (agents[k].statut == I) {
            grille_I[agents[k].y][agents[k].x]++;
        }
    }
}

/* ================================================================
 * VOISINAGE DE MOORE
 * ================================================================ */

/* Retourne le nombre d'individus I dans les 9 cases autour de
 * la position (cx, cy), grille torique. */
int compter_I_voisinage(int cx, int cy) {

    int nb_infectes = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {

            /* Wrap torique : on ajoute GRID_W/H avant le modulo
             * pour eviter les valeurs negatives ((-1) % 300 = -1 en C) */
            int nx = (cx + dx + GRID_W) % GRID_W;
            int ny = (cy + dy + GRID_H) % GRID_H;

            nb_infectes += grille_I[ny][nx];
        }
    }

    return nb_infectes;
}

/* ================================================================
 * UN PAS DE TEMPS (= un jour)
 * ================================================================ */

void simuler_un_jour(Individu *agents) {

    /* --- ETAPE 1 : Deplacement --- */
    /* Chaque individu saute vers une case aleatoire quelconque */
    for (int k = 0; k < NB_AGENTS; k++) {
        Individu *a = &agents[k];
        a->x = rand_int(0, GRID_W - 1);
        a->y = rand_int(0, GRID_H - 1);
    }

    /* --- ETAPE 2 : Reconstruction de la grille --- */
    reconstruire_grille_I(agents);

    /* --- ETAPE 3 : Transitions d'etat --- */
    for (int k = 0; k < NB_AGENTS; k++) {
        Individu *a = &agents[k];

        a->timer++;

        switch (a->statut) {

            case S: {
                int Ni = compter_I_voisinage(a->x, a->y);
                if (Ni > 0) {
                    double p = 1.0 - exp(-FORCE_INFECTION * Ni);
                    if (rand01() < p) {
                        a->statut = E;
                        a->timer  = 0;
                    }
                }
                break;
            }

            case E:
                if (a->timer > a->dE) {
                    a->statut = I;
                    a->timer  = 0;
                    grille_I[a->y][a->x]++;
                }
                break;

            case I:
                if (a->timer > a->dI) {
                    a->statut = R;
                    a->timer  = 0;
                    grille_I[a->y][a->x]--;
                }
                break;

            case R:
                if (a->timer > a->dR) {
                    a->statut = S;
                    a->timer  = 0;
                }
                break;
        }
    }
}

/* ================================================================
 * COMPTAGE DES ETATS
 * ================================================================ */

void compter_etats(Individu *agents, int *nS, int *nE, int *nI, int *nR) {
    *nS = 0;
    *nE = 0;
    *nI = 0;
    *nR = 0;

    for (int k = 0; k < NB_AGENTS; k++) {
        switch (agents[k].statut) {
            case S: (*nS)++; break;
            case E: (*nE)++; break;
            case I: (*nI)++; break;
            case R: (*nR)++; break;
        }
    }
}

/* ================================================================
 * MAIN
 * ================================================================ */

int main(void) {

    srand(SEED);

    Individu *agents = malloc(NB_AGENTS * sizeof(Individu));
    if (agents == NULL) {
        printf("Erreur : malloc a echoue\n");
        return 1;
    }

    initialiser_agents(agents);

    //printf("jour,S,E,I,R\n");

    struct timespec debut, fin;
    clock_gettime(CLOCK_MONOTONIC, &debut);

    for (int jour = 0; jour < NB_JOURS; jour++) {
        int nS, nE, nI, nR;
        compter_etats(agents, &nS, &nE, &nI, &nR);
        //printf("%d,%d,%d,%d,%d\n", jour, nS, nE, nI, nR);
        simuler_un_jour(agents);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double temps = (fin.tv_sec  - debut.tv_sec)
                 + (fin.tv_nsec - debut.tv_nsec) * 1e-9;
    fprintf(stderr, "Temps total : %.3f secondes pour %d jours\n",
            temps, NB_JOURS);

    free(agents);
    return 0;
    /*
    FILE *f = fopen("resultats.csv", "w");
    fprintf(f, "jour,S,E,I,R\n");

    for (int jour = 0; jour < NB_JOURS; jour++) {
        int nS, nE, nI, nR;

        compter_etats(agents, &nS, &nE, &nI, &nR);

        fprintf(f, "%d,%d,%d,%d,%d\n", jour, nS, nE, nI, nR);

        simuler_un_jour(agents);
    }

    fclose(f);
    */
}
