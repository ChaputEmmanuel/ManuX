/*
 * ManuX/Arcade - Pong, console ANSI 80x25.
 * Concu avec l'aide de ChatGPT (OpenAI), 2026.
 * SPDX-License-Identifier: 0BSD
 *
 * Fonctions externes sous ManuX : printf, lire, attendreMillisecondes.
 * Commandes ANSI : CUP et SGR.
 */
#include <stdio.h>
#include <manux/config.h>
#include <manux/types.h>
#include <unistd.h>

#define PONG_GAUCHE  3
#define PONG_DROITE  78
#define PONG_HAUT    3
#define PONG_BAS     22
#define PONG_RAQUETTE 5
#define PONG_PAS_MS  20
#define PONG_BALLE_TICKS 4
#define PONG_SCORE_MAX 7

#define PONG_RESET "\033[0m"
#define PONG_CYAN "\033[36m"
#define PONG_JAUNE "\033[33m"
#define PONG_VERT "\033[32m"
#define PONG_BLANC "\033[97m"

static int pongJoueurY, pongOrdinateurY;
static int pongBalleX, pongBalleY, pongDX, pongDY;
static int pongScoreJoueur, pongScoreOrdinateur;
static int pongQuitter;

static void pongPosition(int y, int x)
{
    printf("\033[%d;%dH", y, x);
}

static void pongPause(void)
{
    attendreMillisecondes(PONG_PAS_MS);
}

static void pongRafraichir(void)
{
#ifndef __MANUX__
    fflush(stdout);
#endif
}

static void pongEffacer(void)
{
    for (int y = 1; y <= 25; y++) {
        pongPosition(y, 1);
        for (int x = 1; x < 80; x++)
            printf(" ");
    }
    pongRafraichir();
}

static void pongScore(void)
{
    pongPosition(1, 22);
    printf(PONG_BLANC "PONG   Vous : %d    ManuX : %d" PONG_RESET,
           pongScoreJoueur, pongScoreOrdinateur);
}

static void pongDecor(void)
{
    pongEffacer();
    pongScore();
    for (int x = PONG_GAUCHE; x <= PONG_DROITE; x++) {
        pongPosition(PONG_HAUT, x);
        printf(PONG_CYAN "-" PONG_RESET);
        pongPosition(PONG_BAS, x);
        printf(PONG_CYAN "-" PONG_RESET);
    }
    for (int y = PONG_HAUT + 1; y < PONG_BAS; y++) {
        pongPosition(y, PONG_GAUCHE);
        printf(PONG_CYAN "|" PONG_RESET);
        pongPosition(y, PONG_DROITE);
        printf(PONG_CYAN "|" PONG_RESET);
        if (y % 2 == 0) {
            pongPosition(y, (PONG_GAUCHE + PONG_DROITE) / 2);
            printf(":");
        }
    }
    pongPosition(24, 9);
    printf("j : monter   k : descendre   q : quitter   Premier a 7");
    pongRafraichir();
}

static void pongRaquette(int x, int y, int visible, const char *couleur)
{
    for (int i = 0; i < PONG_RAQUETTE; i++) {
        pongPosition(y + i, x);
        if (visible)
            printf("%s#%s", couleur, PONG_RESET);
        else
            printf(" ");
    }
}

static void pongBalle(int visible)
{
    pongPosition(pongBalleY, pongBalleX);
    if (visible)
        printf(PONG_BLANC "O" PONG_RESET);
    else
        printf(" ");
}

static void pongRecentrer(int direction)
{
    pongBalleX = (PONG_GAUCHE + PONG_DROITE) / 2;
    pongBalleY = (PONG_HAUT + PONG_BAS) / 2;
    pongDX = direction;
    pongDY = 1;
}

static void pongInitialiser(void)
{
    pongJoueurY = 11;
    pongOrdinateurY = 11;
    pongScoreJoueur = 0;
    pongScoreOrdinateur = 0;
    pongQuitter = 0;
    pongRecentrer(-1);
}

static void pongDeplacerJoueur(int delta)
{
    int y = pongJoueurY + delta;
    if (y < PONG_HAUT + 1)
        y = PONG_HAUT + 1;
    if (y > PONG_BAS - PONG_RAQUETTE)
        y = PONG_BAS - PONG_RAQUETTE;
    if (y != pongJoueurY) {
        pongRaquette(PONG_GAUCHE + 2, pongJoueurY, 0, PONG_VERT);
        pongJoueurY = y;
        pongRaquette(PONG_GAUCHE + 2, pongJoueurY, 1, PONG_VERT);
    }
}

static void pongDeplacerOrdinateur(void)
{
    int centre = pongOrdinateurY + PONG_RAQUETTE / 2;
    int cible = pongBalleY;
    int y = pongOrdinateurY;
    if (cible < centre)
        y--;
    else if (cible > centre)
        y++;
    if (y < PONG_HAUT + 1)
        y = PONG_HAUT + 1;
    if (y > PONG_BAS - PONG_RAQUETTE)
        y = PONG_BAS - PONG_RAQUETTE;
    if (y != pongOrdinateurY) {
        pongRaquette(PONG_DROITE - 2, pongOrdinateurY, 0, PONG_JAUNE);
        pongOrdinateurY = y;
        pongRaquette(PONG_DROITE - 2, pongOrdinateurY, 1, PONG_JAUNE);
    }
}

static void pongAvancerBalle(void)
{
    int nx = pongBalleX + pongDX;
    int ny = pongBalleY + pongDY;

    pongBalle(0);
    /* La ligne centrale est redessinee si la balle l'a effacee. */
    if (pongBalleX == (PONG_GAUCHE + PONG_DROITE) / 2 &&
        pongBalleY % 2 == 0) {
        pongPosition(pongBalleY, pongBalleX);
        printf(":");
    }

    if (ny <= PONG_HAUT || ny >= PONG_BAS) {
        pongDY = -pongDY;
        ny = pongBalleY + pongDY;
    }

    if (pongDX < 0 && nx == PONG_GAUCHE + 2 &&
        ny >= pongJoueurY && ny < pongJoueurY + PONG_RAQUETTE) {
        pongDX = 1;
        pongDY = (ny < pongJoueurY + 2) ? -1 : 1;
        nx = pongBalleX + pongDX;
    }
    if (pongDX > 0 && nx == PONG_DROITE - 2 &&
        ny >= pongOrdinateurY && ny < pongOrdinateurY + PONG_RAQUETTE) {
        pongDX = -1;
        pongDY = (ny < pongOrdinateurY + 2) ? -1 : 1;
        nx = pongBalleX + pongDX;
    }

    if (nx <= PONG_GAUCHE) {
        pongScoreOrdinateur++;
        pongScore();
        pongRecentrer(1);
    } else if (nx >= PONG_DROITE) {
        pongScoreJoueur++;
        pongScore();
        pongRecentrer(-1);
    } else {
        pongBalleX = nx;
        pongBalleY = ny;
    }
    pongBalle(1);
}

int pongJouer(void)
{
    int ticks = 0;
    int aiTicks = 0;
    char touche;
    pongInitialiser();
    pongDecor();
    pongRaquette(PONG_GAUCHE + 2, pongJoueurY, 1, PONG_VERT);
    pongRaquette(PONG_DROITE - 2, pongOrdinateurY, 1, PONG_JAUNE);
    pongBalle(1);
    pongRafraichir();

    while (!pongQuitter &&
           pongScoreJoueur < PONG_SCORE_MAX &&
           pongScoreOrdinateur < PONG_SCORE_MAX) {
        /* On traite au plus 16 touches par iteration pour eviter la famine. */
        for (int i = 0; i < 16; i++) {
            long n = lire(0, &touche, 1);
            if (n != 1)
                break;
            if (touche == 'j')
                pongDeplacerJoueur(-1);
            else if (touche == 'k')
                pongDeplacerJoueur(1);
            else if (touche == 'q') {
                pongQuitter = 1;
                break;
            }
        }
        if (++aiTicks >= 8) {
            aiTicks = 0;
            pongDeplacerOrdinateur();
        }
        if (++ticks >= PONG_BALLE_TICKS) {
            ticks = 0;
            pongAvancerBalle();
        }
        pongRafraichir();
        pongPause();
    }

    pongPosition(24, 9);
    if (pongQuitter)
        printf("Partie interrompue.                                      ");
    else if (pongScoreJoueur == PONG_SCORE_MAX)
        printf("Victoire !                                                ");
    else
        printf("ManuX gagne !                                             ");
    pongPosition(25, 1);
    pongRafraichir();
    return 0;
}
