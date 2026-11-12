#include <stdio.h>

typedef void (*arcadeFonction)(void);

typedef struct {
    const char *nom;
    arcadeFonction fonction;
} arcadeEntree;

/* Fonctions des jeux */
void tetrisMain(void);
void resoudreHanoi(void);
void breakoutMain(void);
void pongJouer(void);
void snakeJouer(void);

/* Menu statique */
static const arcadeEntree arcadeMenu[] = {
    { "Tours de Hanoi", resoudreHanoi },
    { "Tetris",         tetrisMain},
    { "Breakout",       breakoutMain },
    { "Pong",           pongJouer },
    { "Snake",          snakeJouer },
};

#define ARCADE_NB_JEUX \
    (sizeof(arcadeMenu) / sizeof(arcadeMenu[0]))

/* Positionnement ANSI */
static void arcadePosition(int ligne, int colonne)
{
    printf("\033[%d;%dH", ligne, colonne);
}

/* Effacement de l'ecran 80x25 */
static void arcadeEffacer(void)
{
    for (int y = 1; y <= 25; y++) {
        arcadePosition(y, 1);
        for (int x = 1; x <= 79; x++)
            printf(" ");
    }
}

/* Affichage du menu */
static void arcadeAfficher(int selection)
{
    arcadeEffacer();

    arcadePosition(4, 30);
    printf("=== ManuX / Arcade ===");

    for (unsigned int i = 0; i < ARCADE_NB_JEUX; i++) {
        arcadePosition(8 + i * 2, 28);
        printf("%c %s", i == (unsigned int)selection ? '>' : ' ',
               arcadeMenu[i].nom);
    }

    arcadePosition(20, 22);
    printf("j : haut   k : bas   Entree : jouer");
}

/* Selection d'un jeu */
void arcadeChoisir(void)
{
    int selection = 0;
    char touche;

    arcadeAfficher(selection);

    for (;;) {
        if (lire(0, &touche, 1) != 1)
            continue;

        if (touche == 'j') {
            selection = (selection + ARCADE_NB_JEUX - 1) % ARCADE_NB_JEUX;
            arcadeAfficher(selection);
        }
        else if (touche == 'k') {
            selection = (selection + 1) % ARCADE_NB_JEUX;
            arcadeAfficher(selection);
        }
        else if (touche == '\n' || touche == '\r' || touche == ' ') {
            arcadeEffacer();
            arcadeMenu[selection].fonction();
            arcadeAfficher(selection);
        }
    }
}
