/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de manipulation des dates et durées. */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/types.h>

/*
 * Conversion d'une date/durée exprimée en nombre de top horloges vers
 * une structure ValTemps. 
 */
int nbTopVersValTemps(Temps nbTop, ValTemps * vt);

#define totalSecondesDansTemps(t) (t / MANUX_FREQUENCE_TIMER)

#define totalMinutesDansTemps(t) (totalSecondesDansTemps(t)/60)

#define secondesDansTemps(t) (totalSecondesDansTemps(t) - 60*(totalMinutesDansTemps(t)))
