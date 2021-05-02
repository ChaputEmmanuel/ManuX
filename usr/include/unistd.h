#ifndef _UNISTD_H
#define _UNISTD_H

#include <manux/types.h>
#include <manux/appelsysteme.h>

appelSysteme2(NBAS_ECRIRE_CONS, int, ecrireConsole, void *, int);
appelSysteme3(NBAS_ECRIRE, int, ecrire, int, void *, int);
appelSysteme0(NBAS_FORK, TacheID, fork);

#endif
