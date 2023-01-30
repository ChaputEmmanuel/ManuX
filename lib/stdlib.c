/**
 *                                                  (C) Manu Chaput 2020-2023 */
#include <manux/stdlib.h>

static uint32_t valeurAleatoire = 4832;
uint32_t factMult = 742938285;

/**
 * @brief Génération d'un nombre "aléatoire" entre 0 et RAND_MAX 
 */
uint32_t rand()
{
   valeurAleatoire = valeurAleatoire * factMult;
   return valeurAleatoire % (RAND_MAX + 1);
}

/**
 * @brief Modification de la graine du générateur "aléatoire"
 */
void srand(uint32_t graine)
{
   valeurAleatoire = graine;
}
