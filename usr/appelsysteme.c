/*----------------------------------------------------------------------------*/
/*      Implantation de la partie utilisateur des  appels système de ManuX.   */
/* Idéalement, ces fonctions doivent être dispatchées dans des fichiers plus  */
/* appropriés.                                                                */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/types.h>
#include <manux/appelsysteme.h>

typedef void (CorpsTache());

appelSysteme0(NBAS_BASCULER_TACHE, int, basculerTache);
appelSysteme2(NBAS_CREER_TACHE, int, creerNouvelleTache, CorpsTache, booleen);
