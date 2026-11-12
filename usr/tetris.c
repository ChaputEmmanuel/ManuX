/*
 * Mini Tetris ASCII pour ManuX (console ANSI 80x25).
 * Concu avec l'aide de ChatGPT (OpenAI), 2026.
 * SPDX-License-Identifier: 0BSD
 *
 * ManuX : printf, lire (non bloquante), attendreMillisecondes.
 * Linux : lancer avec : stty -icanon -echo min 0 time 0
 *         puis restaurer avec : stty sane
 */
#include <manux/config.h>
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>

#define TETRIS_LARGEUR 10
#define TETRIS_HAUTEUR 16
#define TETRIS_X 26
#define TETRIS_Y 3
#define TETRIS_PAUSE_MS 100
#define TETRIS_TICKS_DESCENTE 5

static unsigned char tetrisGrille[TETRIS_HAUTEUR][TETRIS_LARGEUR];
static int tetrisType, tetrisRotation, tetrisPieceX, tetrisPieceY;
static int tetrisScore, tetrisLignes, tetrisNumero;
static unsigned int tetrisHasard = 12345;

/* Chaque piece est definie par quatre cases dans un carre 4x4. */
static const signed char tetrisFormes[7][4][2] = {
    {{0,1},{1,1},{2,1},{3,1}}, /* I */
    {{1,0},{2,0},{1,1},{2,1}}, /* O */
    {{1,0},{0,1},{1,1},{2,1}}, /* T */
    {{1,0},{2,0},{0,1},{1,1}}, /* S */
    {{0,0},{1,0},{1,1},{2,1}}, /* Z */
    {{0,0},{0,1},{1,1},{2,1}}, /* J */
    {{2,0},{0,1},{1,1},{2,1}}  /* L */
};

static void tetrisPosition(int ligne, int colonne)
{
    printf("\033[%d;%dH", ligne, colonne);
}

static void tetrisRafraichir(void)
{
#ifndef __MANUX__
   fflush(stdout);
#endif
}

static void tetrisAttendre(void)
{
    attendreMillisecondes(TETRIS_PAUSE_MS);

}

static void tetrisEffacerEcran(void)
{
    for (int y = 1; y <= 25; ++y) {
        tetrisPosition(y, 1);
        for (int x = 1; x <= 79; ++x) printf(" ");
    }
    tetrisRafraichir();
}

/* Calcule la position d'une case apres rotation dans le carre 4x4. */
static void tetrisCase(int type, int rotation, int indice, int *x, int *y)
{
    int a = tetrisFormes[type][indice][0];
    int b = tetrisFormes[type][indice][1];
    for (int r = 0; r < rotation; ++r) {
        int tmp = a;
        a = 3 - b;
        b = tmp;
    }
    *x = a;
    *y = b;
}

static int tetrisCollision(int type, int rotation, int px, int py)
{
    for (int i = 0; i < 4; ++i) {
        int x, y;
        tetrisCase(type, rotation, i, &x, &y);
        x += px;
        y += py;
        if (x < 0 || x >= TETRIS_LARGEUR || y >= TETRIS_HAUTEUR)
            return 1;
        if (y >= 0 && tetrisGrille[y][x]) return 1;
    }
    return 0;
}

static void tetrisDecor(void)
{
    tetrisEffacerEcran();
    tetrisPosition(1, TETRIS_X - 1);
    printf("T E T R I S");
    for (int y = 0; y < TETRIS_HAUTEUR; ++y) {
        tetrisPosition(TETRIS_Y + y, TETRIS_X - 2);
        printf("|");
        tetrisPosition(TETRIS_Y + y, TETRIS_X + 2 * TETRIS_LARGEUR);
        printf("|");
    }
    tetrisPosition(TETRIS_Y + TETRIS_HAUTEUR, TETRIS_X - 2);
    printf("+");
    for (int x = 0; x < 2 * TETRIS_LARGEUR + 1; ++x) printf("-");
    printf("+");
    tetrisPosition(4, 52); printf("j : gauche");
    tetrisPosition(5, 52); printf("k : tourner");
    tetrisPosition(6, 52); printf("l : droite");
    tetrisPosition(7, 52); printf("s : descendre");
    tetrisPosition(8, 52); printf("espace : chute");
    tetrisPosition(9, 52); printf("q : quitter");
    tetrisRafraichir();
}

static void tetrisDessinerPiece(int type, int rotation, int px, int py, char symbole)
{
    for (int i = 0; i < 4; ++i) {
        int x, y;
        tetrisCase(type, rotation, i, &x, &y);
        x += px;
        y += py;
        if (x >= 0 && x < TETRIS_LARGEUR && y >= 0 && y < TETRIS_HAUTEUR) {
            tetrisPosition(TETRIS_Y + y, TETRIS_X + 2 * x);
            printf("%c%c", symbole, symbole);
        }
    }
}

static void tetrisDessinerGrille(void)
{
    for (int y = 0; y < TETRIS_HAUTEUR; ++y) {
        tetrisPosition(TETRIS_Y + y, TETRIS_X);
        for (int x = 0; x < TETRIS_LARGEUR; ++x)
            printf(tetrisGrille[y][x] ? "##" : "  ");
    }
    tetrisDessinerPiece(tetrisType, tetrisRotation,
                        tetrisPieceX, tetrisPieceY, '#');
    tetrisPosition(12, 52); printf("Score  : %-8d", tetrisScore);
    tetrisPosition(13, 52); printf("Lignes : %-8d", tetrisLignes);
    tetrisRafraichir();
}

static void tetrisSupprimerLignes(void)
{
    for (int y = TETRIS_HAUTEUR - 1; y >= 0; --y) {
        int pleine = 1;
        for (int x = 0; x < TETRIS_LARGEUR; ++x)
            if (!tetrisGrille[y][x]) pleine = 0;
        if (pleine) {
            for (int yy = y; yy > 0; --yy)
                for (int x = 0; x < TETRIS_LARGEUR; ++x)
                    tetrisGrille[yy][x] = tetrisGrille[yy - 1][x];
            for (int x = 0; x < TETRIS_LARGEUR; ++x)
                tetrisGrille[0][x] = 0;
            ++tetrisLignes;
            tetrisScore += 100;
            ++y; /* Reexaminer la meme ligne apres decalage. */
        }
    }
}

static int tetrisNouvellePiece(void)
{
    /* Suite deterministe pseudo-aleatoire, sans rand(). */
    tetrisHasard = tetrisHasard * 1664525U + 1013904223U;
    tetrisType = (int)((tetrisHasard >> 16) % 7U);
    tetrisRotation = 0;
    tetrisPieceX = TETRIS_LARGEUR / 2 - 2;
    tetrisPieceY = -1;
    ++tetrisNumero;
    return !tetrisCollision(tetrisType, tetrisRotation,
                            tetrisPieceX, tetrisPieceY);
}

/* Retourne 0 si la partie est terminee. */
static int tetrisDescendre(void)
{
    if (!tetrisCollision(tetrisType, tetrisRotation,
                         tetrisPieceX, tetrisPieceY + 1)) {
        ++tetrisPieceY;
        return 1;
    }
    for (int i = 0; i < 4; ++i) {
        int x, y;
        tetrisCase(tetrisType, tetrisRotation, i, &x, &y);
        x += tetrisPieceX;
        y += tetrisPieceY;
        if (y < 0) return 0;
        tetrisGrille[y][x] = 1;
    }
    tetrisSupprimerLignes();
    return tetrisNouvellePiece();
}

static void tetrisTourner(void)
{
    int rotation = (tetrisRotation + 1) % 4;
    if (!tetrisCollision(tetrisType, rotation, tetrisPieceX, tetrisPieceY))
        tetrisRotation = rotation;
}

static int tetrisCommande(char touche)
{
    if (touche == 'q') return 0;
    if (touche == 'j' &&
        !tetrisCollision(tetrisType, tetrisRotation,
                         tetrisPieceX - 1, tetrisPieceY)) --tetrisPieceX;
    if (touche == 'l' &&
        !tetrisCollision(tetrisType, tetrisRotation,
                         tetrisPieceX + 1, tetrisPieceY)) ++tetrisPieceX;
    if (touche == 'k') tetrisTourner();
    if (touche == 's' && !tetrisDescendre()) return 0;
    if (touche == ' ') {
        while (!tetrisCollision(tetrisType, tetrisRotation,
                                tetrisPieceX, tetrisPieceY + 1))
            ++tetrisPieceY;
        if (!tetrisDescendre()) return 0;
    }
    return 1;
}

void tetrisMain(void)
{
    int actif = 1;
    int ticks = 0;
    tetrisScore = 0;
    tetrisLignes = 0;
    tetrisNumero = 0;
    tetrisDecor();
    actif = tetrisNouvellePiece();
    while (actif) {
        char touche = 0;
        long n = lire(0, &touche, 1);
        if (n == 1) actif = tetrisCommande(touche);
        if (!actif) break;
        if (++ticks >= TETRIS_TICKS_DESCENTE) {
            ticks = 0;
            actif = tetrisDescendre();
        }
        tetrisDessinerGrille();
        tetrisAttendre();
    }
    tetrisDessinerGrille();
    tetrisPosition(21, 24);
    printf("Fin de partie ! Score : %d", tetrisScore);
    tetrisPosition(24, 1);
    tetrisRafraichir();

}

