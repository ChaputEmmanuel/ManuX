/*----------------------------------------------------------------------------*/
/*      Implantation des outils de journalisation des messages du noyau.      */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>  // NULL
#include <manux/journal.h>

/*
 * Pour le moment, le journal est une console virtuelle spécifique.
 */
#ifdef MANUX_JOURNAL_USES_FILES
Fichier * journal = NULL;
#else
Console * journal = NULL;
#endif
//static ExclusionMutuelle emj;

/*
 * Le buffer de stockage de l'écran.
 */
//char bufferJournal[4096];

#ifdef MANUX_JOURNAL_USES_FILES
void initialiserJournal(Fichier * pc)
{
   char * msg = "OK\n";
  
   journal = pc;

   fichierEcrire(journal, msg, 3);
}

#else
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
#endif

void journaliser(char * message, int len)
{
    //   entrerExclusionMutuelle(&emj);

#ifdef MANUX_JOURNAL_USES_FILES
   fichierEcrire(journal, message, len);   
#else
   afficherConsoleN(journal, message, len);
#endif
   
  //   sortirExclusionMutuelle(&emj);
}

void afficherJournal()
{
#ifdef MANUX_CONSOLES_VIRTUELLES
   basculerVersConsole(journal);
#endif
}
