/*----------------------------------------------------------------------------*/
/*      Définition des opérations spécifiques au processeur Intel.            */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef I386_DEF
#define I386_DEF

void halt();
/*
 * Arret complet du système
 *
 * WARNING : codé dans interBasNiveau ...
 */

#define str(tr) \
   __asm__("str %0": "=g"(tr));


#define ltr(tr) \
   __asm__ __volatile__ ("ltr %%ax"::"a"(tr));

#endif
