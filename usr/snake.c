/*
 * ManuX/Arcade : Snake ANSI 80x25
 * Concu avec l'aide de ChatGPT (OpenAI), 2026.
 * SPDX-License-Identifier: 0BSD
 */
#include <manux/config.h>
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>

#define LARGEUR 50
#define HAUTEUR 18
#define CAPACITE (LARGEUR * HAUTEUR)
#define X0 15
#define Y0 3
#define PAS_MS 10
#define TICKS_MOUVEMENT 7
#define RESET "\033[0m"
#define VERT "\033[32m"
#define JAUNE "\033[33m"
#define ROUGE "\033[31m"
#define CYAN "\033[36m"

typedef struct { int x, y; } snakePoint;
static snakePoint snakeCorps[CAPACITE], snakeFruit;
static int snakeDebut, snakeLongueur, snakeDX, snakeDY;
static int snakeProchainDX, snakeProchainDY, snakeScore;
static unsigned int snakeGraine = 123456789u;

static void snakePosition(int y, int x)
{
    printf("\033[%d;%dH", y, x);
}

static void snakePause(void)
{
    attendreMillisecondes(PAS_MS);
}

static void snakeRafraichir(void)
{
#ifndef __MANUX__
   fflush(stdout);
#endif
}

static void snakeAfficher(int x, int y, char c, const char *couleur)
{
    snakePosition(Y0 + y, X0 + x);
    printf("%s%c" RESET, couleur, c);
}

static snakePoint snakeSegment(int i)
{
    return snakeCorps[(snakeDebut + i) % CAPACITE];
}

static int snakeOccupe(int x, int y, int ignorerQueue)
{
    int n = snakeLongueur - ignorerQueue;
    for (int i = 0; i < n; ++i) {
        snakePoint p = snakeSegment(i);
        if (p.x == x && p.y == y) return 1;
    }
    return 0;
}

static void snakeScoreAfficher(void)
{
    snakePosition(1, 24);
    printf(JAUNE "SNAKE   Score : %5d" RESET, snakeScore);
}

static void snakeDecor(void)
{
    for (int y = 1; y <= 25; ++y) {
        snakePosition(y, 1);
        for (int x = 1; x < 80; ++x) printf(" ");
    }
    for (int x = -1; x <= LARGEUR; ++x) {
        snakeAfficher(x, -1, '-', CYAN);
        snakeAfficher(x, HAUTEUR, '-', CYAN);
    }
    for (int y = 0; y < HAUTEUR; ++y) {
        snakeAfficher(-1, y, '|', CYAN);
        snakeAfficher(LARGEUR, y, '|', CYAN);
    }
    snakeScoreAfficher();
    snakePosition(23, 12);
    printf("i : haut   k : bas   j : gauche   l : droite   q : quitter");
}

static void snakePlacerFruit(void)
{
    snakeGraine = snakeGraine * 1664525u + 1013904223u;
    int debut = (int)(snakeGraine % CAPACITE);
    for (int i = 0; i < CAPACITE; ++i) {
        int p = (debut + i) % CAPACITE;
        int x = p % LARGEUR, y = p / LARGEUR;
        if (!snakeOccupe(x, y, 0)) {
            snakeFruit.x = x;
            snakeFruit.y = y;
            snakeAfficher(x, y, '@', ROUGE);
            return;
        }
    }
}

static void snakeInitialiser(void)
{
    snakeDebut = 0;
    snakeLongueur = 4;
    snakeDX = snakeProchainDX = 1;
    snakeDY = snakeProchainDY = 0;
    snakeScore = 0;
    snakeGraine += 977u;
    for (int i = 0; i < snakeLongueur; ++i) {
        snakeCorps[i].x = LARGEUR / 2 - i;
        snakeCorps[i].y = HAUTEUR / 2;
    }
}

static int snakeLireClavier(void)
{
    char c;
    for (int i = 0; i < 16; ++i) {
        if (lire(0, &c, 1) != 1) break;
        int dx = snakeProchainDX, dy = snakeProchainDY;
        if (c == 'q') return 1;
        if (c == 'i') { dx = 0; dy = -1; }
        else if (c == 'k') { dx = 0; dy = 1; }
        else if (c == 'j') { dx = -1; dy = 0; }
        else if (c == 'l') { dx = 1; dy = 0; }
        else continue;
        if (dx != -snakeDX || dy != -snakeDY) {
            snakeProchainDX = dx;
            snakeProchainDY = dy;
        }
    }
    return 0;
}

/* Retour : 0 = continue, 1 = collision, 2 = terrain rempli. */
static int snakeAvancer(void)
{
    snakeDX = snakeProchainDX;
    snakeDY = snakeProchainDY;
    snakePoint tete = snakeSegment(0);
    int x = tete.x + snakeDX, y = tete.y + snakeDY;
    int mange = x == snakeFruit.x && y == snakeFruit.y;

    if (x < 0 || x >= LARGEUR || y < 0 || y >= HAUTEUR ||
        snakeOccupe(x, y, !mange)) return 1;

    snakePoint queue = snakeSegment(snakeLongueur - 1);
    if (!mange) snakeAfficher(queue.x, queue.y, ' ', RESET);

    snakeDebut = (snakeDebut + CAPACITE - 1) % CAPACITE;
    snakeCorps[snakeDebut].x = x;
    snakeCorps[snakeDebut].y = y;

    if (mange) {
        ++snakeLongueur;
        snakeScore += 10;
        snakeScoreAfficher();
    }
    snakeAfficher(tete.x, tete.y, 'o', VERT);
    snakeAfficher(x, y, 'O', JAUNE);

    if (mange) {
        if (snakeLongueur == CAPACITE) return 2;
        snakePlacerFruit();
    }
    return 0;
}

void snakeJouer(void)
{
    int ticks = 0, resultat = 0, quitter = 0;
    snakeInitialiser();
    snakeDecor();
    for (int i = 0; i < snakeLongueur; ++i) {
        snakePoint p = snakeSegment(i);
        snakeAfficher(p.x, p.y, i ? 'o' : 'O', i ? VERT : JAUNE);
    }
    snakePlacerFruit();
    snakeRafraichir();

    while (!resultat && !quitter) {
        quitter = snakeLireClavier();
        if (quitter) break;
        if (++ticks >= TICKS_MOUVEMENT) {
            ticks = 0;
            resultat = snakeAvancer();
        }
        snakeRafraichir();
        snakePause();
    }
    snakePosition(24, 20);
    if (quitter) printf("Partie interrompue.");
    else if (resultat == 2) printf("Victoire : terrain rempli !");
    else printf("Perdu ! Score : %d", snakeScore);
    snakePosition(25, 1);
    snakeRafraichir();
}
