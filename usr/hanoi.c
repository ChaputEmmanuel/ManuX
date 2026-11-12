/*
 * Tours de Hanoi animees et interactives pour ManuX/Arcade (ANSI 80x25).
 * Base : version adaptee pour ManuX par l'utilisateur.
 * Concu avec l'aide de ChatGPT (OpenAI), 2026.
 * SPDX-License-Identifier: 0BSD
 */
#include <manux/config.h>
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DISQUES 10
#define NB_LIGNES 25
#define NB_COLONNES 80
#define Y_HAUT 3
#define ESPACEMENT 20
#define COLONNE_MESSAGE 19

static int N = 4;
#define Y_BASE (N + 6)
#define LIGNE_MESSAGE (Y_BASE + 3)

static int tours[3][MAX_DISQUES];
static int hauteurs[3];
static int abandon;
static int modeDemo;

void position(int ligne, int colonne)
{
    printf("\033[%d;%dH", ligne, colonne);
}

void rafraichir(void)
{
#ifdef __linux__
    /* Conserve le choix de la version ManuX : pas de fflush. */
    fflush(stdout);
#endif
}

void pause_animation(void)
{
    attendreMillisecondes(20);
}

void effacer_ecran(void)
{
    for (int y = 1; y <= NB_LIGNES; y++) {
        position(y, 1);
        for (int x = 1; x < NB_COLONNES; x++)
            printf(" ");
    }
    rafraichir();
}

int colonne_tour(int tour)
{
    return 12 + tour * ESPACEMENT;
}

void disque(int taille, int x, int y, int effacer)
{
    position(y, x - taille);
    for (int i = -taille; i <= taille; i++) {
        if (!effacer) {
            printf("=");
        } else {
            int axe = 0;
            if (y >= Y_HAUT && y < Y_BASE)
                for (int t = 0; t < 3; t++)
                    if (x + i == colonne_tour(t))
                        axe = 1;
            printf("%c", axe ? '|' : ' ');
        }
    }
    rafraichir();
}

void effacer_message(void)
{
    position(LIGNE_MESSAGE, 1);
    for (int i = 1; i < NB_COLONNES; i++)
        printf(" ");
}

void message(int taille, int depart, int arrivee)
{
    effacer_message();
    position(LIGNE_MESSAGE, COLONNE_MESSAGE);
    printf("Le disque %d va de la tour %d a la tour %d",
           taille, depart + 1, arrivee + 1);
    rafraichir();
}

static void hanoiInformation(const char *texte)
{
    effacer_message();
    position(LIGNE_MESSAGE, COLONNE_MESSAGE);
    printf("%s", texte);
    rafraichir();
}

void decor(void)
{
    effacer_ecran();
    for (int t = 0; t < 3; t++) {
        int x = colonne_tour(t);
        for (int y = Y_HAUT; y < Y_BASE; y++) {
            position(y, x);
            printf("|");
        }
        position(Y_BASE, x - N);
        for (int i = 0; i < 2 * N + 1; i++)
            printf("-");
    }
    for (int i = 0; i < N; i++)
        disque(tours[0][i], colonne_tour(0), Y_BASE - 1 - i, 0);
    rafraichir();
}

/* Un caractere disponible, ou 0 si aucun. */
static int hanoiLireTouche(char *touche)
{
    return lire(0, touche, 1) == 1;
}

/* Dans la demo, q interrompt meme un mouvement en cours. */
static void hanoiVerifierAbandon(void)
{
    char touche;
    if (modeDemo && hanoiLireTouche(&touche) && touche == 'q')
        abandon = 1;
}

/* Attend une touche sans faire d'hypothese sur le buffering clavier. */
static char hanoiAttendreTouche(void)
{
    char touche;
    while (!hanoiLireTouche(&touche))
        attendreMillisecondes(20);
    return touche;
}

/* Retourne 0 si le mouvement est abandonne en cours d'animation. */
int deplacer(int depart, int arrivee)
{
    int taille = tours[depart][--hauteurs[depart]];
    int x = colonne_tour(depart);
    int cible = colonne_tour(arrivee);
    int y = Y_BASE - 1 - hauteurs[depart];
    int destination = Y_BASE - 1 - hauteurs[arrivee];

    message(taille, depart, arrivee);

    while (y > 2) {
        disque(taille, x, y, 1);
        y--;
        disque(taille, x, y, 0);
        pause_animation();
        hanoiVerifierAbandon();
        if (abandon) return 0;
    }
    while (x != cible) {
        disque(taille, x, y, 1);
        x += (x < cible) ? 1 : -1;
        disque(taille, x, y, 0);
        pause_animation();
        hanoiVerifierAbandon();
        if (abandon) return 0;
    }
    while (y < destination) {
        disque(taille, x, y, 1);
        y++;
        disque(taille, x, y, 0);
        pause_animation();
        hanoiVerifierAbandon();
        if (abandon) return 0;
    }
    tours[arrivee][hauteurs[arrivee]++] = taille;
    return 1;
}

void hanoi(int n, int depart, int arrivee, int auxiliaire)
{
    if (n == 0 || abandon)
        return;
    hanoi(n - 1, depart, auxiliaire, arrivee);
    if (abandon) return;
    hanoiVerifierAbandon();
    if (abandon || !deplacer(depart, arrivee)) return;
    hanoi(n - 1, auxiliaire, arrivee, depart);
}

static void hanoiInitialiser(void)
{
    for (int t = 0; t < 3; t++) {
        hauteurs[t] = 0;
        for (int i = 0; i < MAX_DISQUES; i++)
            tours[t][i] = 0;
    }
    hauteurs[0] = N;
    for (int i = 0; i < N; i++)
        tours[0][i] = N - i;
    abandon = 0;
}

/* Choix des options : une seule touche suffit, pas besoin d'Entree. */
static void hanoiConfigurer(void)
{
    char touche;
    effacer_ecran();
    position(7, 24);
    printf("=== ManuX / Arcade : Hanoi ===");
    position(10, 18);
    printf("Nombre de disques : 3 a 9, ou 0 pour 10");
    do {
        touche = hanoiAttendreTouche();
    } while (!((touche >= '3' && touche <= '9') || touche == '0'));
    N = touche == '0' ? 10 : touche - '0';

    position(12, 18);
    printf("Mode : d = demonstration, j = jeu");
    do {
        touche = hanoiAttendreTouche();
    } while (touche != 'd' && touche != 'j');
    modeDemo = touche == 'd';
}

static int hanoiMouvementValide(int depart, int arrivee)
{
    if (depart == arrivee || hauteurs[depart] == 0)
        return 0;
    if (hauteurs[arrivee] == 0)
        return 1;
    return tours[depart][hauteurs[depart] - 1] <
           tours[arrivee][hauteurs[arrivee] - 1];
}

static void hanoiJouer(void)
{
    int depart = -1;
    char touche;
    position(LIGNE_MESSAGE + 2, 12);
    printf("Tours : 1, 2, 3  |  q : quitter");
    hanoiInformation("Choisir la tour de depart (1-3)");

    while (!abandon && hauteurs[2] != N) {
        touche = hanoiAttendreTouche();
        if (touche == 'q') {
            abandon = 1;
            break;
        }
        if (touche < '1' || touche > '3')
            continue;
        if (depart < 0) {
            depart = touche - '1';
            if (hauteurs[depart] == 0) {
                hanoiInformation("Cette tour est vide !");
                depart = -1;
            } else {
                hanoiInformation("Choisir la tour d'arrivee (1-3)");
            }
        } else {
            int arrivee = touche - '1';
            if (!hanoiMouvementValide(depart, arrivee)) {
                hanoiInformation("Deplacement interdit ! Recommencer.");
            } else {
                deplacer(depart, arrivee);
            }
            depart = -1;
            if (hauteurs[2] != N)
                hanoiInformation("Choisir la tour de depart (1-3)");
        }
    }
}

int resoudreHanoi(void)
{
    hanoiConfigurer();
    hanoiInitialiser();
    decor();
    if (modeDemo) {
        position(LIGNE_MESSAGE + 2, 12);
        printf("Demonstration : q pour quitter");
        hanoi(N, 0, 2, 1);
    } else {
        hanoiJouer();
    }

    effacer_message();
    position(LIGNE_MESSAGE, 29);
    if (abandon)
        printf("Partie interrompue !");
    else if (modeDemo)
        printf("Resolution terminee !");
    else
        printf("Bravo, partie gagnee !");
    position(NB_LIGNES, 1);
    rafraichir();
    return 0;
}
