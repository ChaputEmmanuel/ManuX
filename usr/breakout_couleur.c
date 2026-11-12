/*
 * ManuX/Arcade - Breakout ASCII couleur 80x25
 * Concu avec l'aide de ChatGPT (OpenAI), 2026.
 * SPDX-License-Identifier: 0BSD
 *
 * ManuX : printf(), lire() non bloquante, attendreMillisecondes().
 * Linux : printf(), read(), fflush(), usleep() (terminal en mode non canonique).
 * Commandes : j gauche, l droite, espace lancement, q quitter.
 */
#include <stdio.h>
#include <unistd.h>


#define BREAKOUT_RESET "\033[0m"
#define BREAKOUT_ROUGE "\033[31m"
#define BREAKOUT_VERT "\033[32m"
#define BREAKOUT_JAUNE "\033[33m"
#define BREAKOUT_BLEU "\033[34m"
#define BREAKOUT_MAGENTA "\033[35m"
#define BREAKOUT_CYAN "\033[36m"
#define BREAKOUT_BLANC "\033[97m"

#define BREAKOUT_COLS 80
#define BREAKOUT_ROWS 25
#define BREAKOUT_GAUCHE 12
#define BREAKOUT_DROITE 68
#define BREAKOUT_HAUT 2
#define BREAKOUT_BAS 21
#define BREAKOUT_RAQUETTE_Y 20
#define BREAKOUT_RAQUETTE_L 9
#define BREAKOUT_BRIQUES_LIGNES 4
#define BREAKOUT_BRIQUES_COLS 9
#define BREAKOUT_BRIQUE_L 5
#define BREAKOUT_BRIQUE_X 17
#define BREAKOUT_BRIQUE_Y 4
#define BREAKOUT_PAS_MS 10

static unsigned char breakoutBriques[BREAKOUT_BRIQUES_LIGNES][BREAKOUT_BRIQUES_COLS];
static int breakoutX, breakoutY, breakoutDx, breakoutDy;
static int breakoutRaquette, breakoutScore, breakoutRestantes, breakoutVies;
static int breakoutLance, breakoutFini;

static void breakoutPosition(int y, int x)
{
    printf("\033[%d;%dH", y, x);
}

static void breakoutRafraichir(void)
{
#ifndef MANUX
  //    fflush(stdout);
#endif
}

static void breakoutAttendre(void)
{
    attendreMillisecondes(BREAKOUT_PAS_MS);
}

static long breakoutLire(char *c)
{
    return lire(0, c, 1);
}

static void breakoutTexte(int y, int x, const char *s)
{
    breakoutPosition(y, x);
    printf("%s", s);
}

static void breakoutEffacerEcran(void)
{
    for (int y = 1; y <= BREAKOUT_ROWS; ++y) {
        breakoutPosition(y, 1);
        for (int x = 1; x < BREAKOUT_COLS; ++x)
            printf(" ");
    }
}

static void breakoutDessinerBrique(int ligne, int col, int visible)
{
    int x = BREAKOUT_BRIQUE_X + col * BREAKOUT_BRIQUE_L;
    int y = BREAKOUT_BRIQUE_Y + ligne;
    breakoutPosition(y, x);
    if (visible) {
        const char *couleurs[] = {BREAKOUT_ROUGE, BREAKOUT_JAUNE,
                                  BREAKOUT_VERT, BREAKOUT_CYAN};
        printf("%s", couleurs[ligne]);
    }
    for (int i = 0; i < BREAKOUT_BRIQUE_L; ++i)
        printf("%c", visible ? '#' : ' ');
    printf(BREAKOUT_RESET);
}

static void breakoutDessinerRaquette(int visible)
{
    breakoutPosition(BREAKOUT_RAQUETTE_Y, breakoutRaquette);
    if (visible) printf(BREAKOUT_MAGENTA);
    for (int i = 0; i < BREAKOUT_RAQUETTE_L; ++i)
        printf("%c", visible ? '=' : ' ');
    printf(BREAKOUT_RESET);
}

static void breakoutDessinerBalle(int visible)
{
    breakoutPosition(breakoutY, breakoutX);
    if (visible) printf(BREAKOUT_BLANC);
    printf("%c", visible ? 'O' : ' ');
    printf(BREAKOUT_RESET);
}

static void breakoutStatut(void)
{
    breakoutPosition(1, 13);
    printf(BREAKOUT_BLANC);
    printf("BREAKOUT  Score: %4d  Vies: %d  Briques: %2d   ",
           breakoutScore, breakoutVies, breakoutRestantes);
    printf(BREAKOUT_RESET);
    breakoutPosition(23, 13);
    if (breakoutFini == 1)
        printf("GAGNE !                          ");
    else if (breakoutFini == 2)
        printf("PERDU !                          ");
    else if (!breakoutLance)
        printf("ESPACE pour lancer la balle      ");
    else
        printf("j: gauche   l: droite   q: quitter");
}

static void breakoutPlacerBalle(void)
{
    breakoutX = breakoutRaquette + BREAKOUT_RAQUETTE_L / 2;
    breakoutY = BREAKOUT_RAQUETTE_Y - 1;
    breakoutDx = 1;
    breakoutDy = -1;
    breakoutLance = 0;
}

static void breakoutInitialiser(void)
{
    breakoutRaquette = (BREAKOUT_GAUCHE + BREAKOUT_DROITE - BREAKOUT_RAQUETTE_L + 1) / 2;
    breakoutScore = 0;
    breakoutVies = 3;
    breakoutRestantes = BREAKOUT_BRIQUES_LIGNES * BREAKOUT_BRIQUES_COLS;
    breakoutFini = 0;
    for (int y = 0; y < BREAKOUT_BRIQUES_LIGNES; ++y)
        for (int x = 0; x < BREAKOUT_BRIQUES_COLS; ++x)
            breakoutBriques[y][x] = 1;
    breakoutPlacerBalle();
}

static void breakoutDecor(void)
{
    breakoutEffacerEcran();
    for (int y = BREAKOUT_HAUT; y <= BREAKOUT_BAS; ++y) {
        breakoutPosition(y, BREAKOUT_GAUCHE);
        printf(BREAKOUT_BLEU "|" BREAKOUT_RESET);
        breakoutPosition(y, BREAKOUT_DROITE);
        printf(BREAKOUT_BLEU "|" BREAKOUT_RESET);
    }
    breakoutPosition(BREAKOUT_HAUT, BREAKOUT_GAUCHE);
    for (int x = BREAKOUT_GAUCHE; x <= BREAKOUT_DROITE; ++x)
        printf(BREAKOUT_BLEU "-" BREAKOUT_RESET);
    for (int y = 0; y < BREAKOUT_BRIQUES_LIGNES; ++y)
        for (int x = 0; x < BREAKOUT_BRIQUES_COLS; ++x)
            breakoutDessinerBrique(y, x, 1);
    breakoutDessinerRaquette(1);
    breakoutDessinerBalle(1);
    breakoutStatut();
    breakoutRafraichir();
}

static void breakoutDeplacerRaquette(int sens)
{
    int nouveau = breakoutRaquette + sens * 2;
    if (nouveau < BREAKOUT_GAUCHE + 1)
        nouveau = BREAKOUT_GAUCHE + 1;
    if (nouveau + BREAKOUT_RAQUETTE_L - 1 >= BREAKOUT_DROITE)
        nouveau = BREAKOUT_DROITE - BREAKOUT_RAQUETTE_L;
    if (nouveau == breakoutRaquette)
        return;
    breakoutDessinerRaquette(0);
    if (!breakoutLance)
        breakoutDessinerBalle(0);
    breakoutRaquette = nouveau;
    breakoutDessinerRaquette(1);
    if (!breakoutLance) {
        breakoutX = breakoutRaquette + BREAKOUT_RAQUETTE_L / 2;
        breakoutDessinerBalle(1);
    }
}

static void breakoutRebondir(void)
{
    int nx = breakoutX + breakoutDx;
    int ny = breakoutY + breakoutDy;

    if (nx <= BREAKOUT_GAUCHE || nx >= BREAKOUT_DROITE) {
        breakoutDx = -breakoutDx;
        nx = breakoutX + breakoutDx;
    }
    if (ny <= BREAKOUT_HAUT) {
        breakoutDy = -breakoutDy;
        ny = breakoutY + breakoutDy;
    }

    /* Collision avec les briques : on teste la prochaine case. */
    if (ny >= BREAKOUT_BRIQUE_Y &&
        ny < BREAKOUT_BRIQUE_Y + BREAKOUT_BRIQUES_LIGNES &&
        nx >= BREAKOUT_BRIQUE_X &&
        nx < BREAKOUT_BRIQUE_X + BREAKOUT_BRIQUES_COLS * BREAKOUT_BRIQUE_L) {
        int lig = ny - BREAKOUT_BRIQUE_Y;
        int col = (nx - BREAKOUT_BRIQUE_X) / BREAKOUT_BRIQUE_L;
        if (breakoutBriques[lig][col]) {
            breakoutBriques[lig][col] = 0;
            breakoutDessinerBrique(lig, col, 0);
            breakoutScore += 10;
            --breakoutRestantes;
            breakoutDy = -breakoutDy;
            ny = breakoutY + breakoutDy;
            nx = breakoutX + breakoutDx;
            if (breakoutRestantes == 0) {
                breakoutFini = 1;
                return;
            }
        }
    }

    if (breakoutDy > 0 && ny == BREAKOUT_RAQUETTE_Y &&
        nx >= breakoutRaquette &&
        nx < breakoutRaquette + BREAKOUT_RAQUETTE_L) {
        int milieu = breakoutRaquette + BREAKOUT_RAQUETTE_L / 2;
        breakoutDy = -1;
        if (nx < milieu)
            breakoutDx = -1;
        else if (nx > milieu)
            breakoutDx = 1;
        ny = breakoutY + breakoutDy;
        nx = breakoutX + breakoutDx;
    }

    if (ny >= BREAKOUT_BAS) {
        breakoutDessinerBalle(0);
        --breakoutVies;
        if (breakoutVies == 0)
            breakoutFini = 2;
        else
            breakoutPlacerBalle();
        breakoutDessinerBalle(1);
        return;
    }

    breakoutDessinerBalle(0);
    breakoutX = nx;
    breakoutY = ny;
    breakoutDessinerBalle(1);
}

void breakoutMain(void)
{
    int compteur = 0;
    breakoutInitialiser();
    breakoutDecor();

    while (!breakoutFini) {
        char touche;
        long n = breakoutLire(&touche);
        if (n < 0)
            break;
        if (n == 1) {
            if (touche == 'q')
                break;
            if (touche == 'j')
                breakoutDeplacerRaquette(-2);
            else if (touche == 'l')
                breakoutDeplacerRaquette(2);
            else if (touche == ' ')
                breakoutLance = 1;
        }
        if (breakoutLance) {
            ++compteur;
            if (compteur >= 2) {
                compteur = 0;
                breakoutRebondir();
            }
        } else {
            compteur = 0;
        }
        breakoutStatut();
        breakoutRafraichir();
        breakoutAttendre();
    }

    breakoutStatut();
    breakoutTexte(24, 13, "Fin du jeu.                         ");
    breakoutPosition(25, 1);
    breakoutRafraichir();
}
