/*----------------------------------------------------------------------------*/
/*      Implantation des outils de journalisation des messages du noyau.      */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>  // NULL
#include <manux/journal.h>

/*
 * Pour le moment, le journal est une console virtuelle spécifique.
 */
static Console * journal;
//static ExclusionMutuelle emj;

/*
 * Le buffer de stockage de l'écran.
 */
char bufferJournal[4096];

void initialiserJournal(Console * console)
{
    //   initialiserExclusionMutuelle(&emj);
    //   entrerExclusionMutuelle(&emj);

   journal = console;
   affecterCouleurTexte(journal, COUL_TXT_BLANC);

   // Affichons un petit message   
   afficherConsole(journal, "Journal de ManuX-32\n");

   //   sortirExclusionMutuelle(&emj);
}

void journaliser(char * message)
{
    //   entrerExclusionMutuelle(&emj);

  afficherConsole(journal, message);

  //   sortirExclusionMutuelle(&emj);
}

void afficherJournal()
{
#ifdef MANUX_CONSOLES_VIRTUELLES
   basculerVersConsole(journal);
#endif
}
