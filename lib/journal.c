/*----------------------------------------------------------------------------*/
/*      Implantation des outils de journalisation des messages du noyau.      */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>  // NULL
#include <manux/journal.h>
#include <manux/console.h>

/*
 * Pour le moment, le journal est une console virtuelle spécifique.
 */
static Console journal;
//static ExclusionMutuelle emj;

/*
 * Le buffer de stockage de l'écran.
 */
char bufferJournal[4096];

void initialiserJournal()
{
    //   initialiserExclusionMutuelle(&emj);
    //   entrerExclusionMutuelle(&emj);

#ifdef CONSOLES_VIRTUELLES

   // Si l'on utilise des consoles virtuelles, il faut initialiser
   // celle du journal
  
   initialiserConsole(&journal, bufferJournal);
   affecterCouleurTexte(&journal, COUL_BLANC);

   // Affichons un petit message   
   afficherConsole(&journal, "Journal de ManuX-32\n");
#else
   afficherEcran("Journal de ManuX-32\n");   
#endif // CONSOLES_VIRTUELLES

   
   //   sortirExclusionMutuelle(&emj);
}

void journaliser(char * message)
{
    //   entrerExclusionMutuelle(&emj);

#ifdef CONSOLES_VIRTUELLES
  afficherConsole(&journal, message);
#else
  afficherEcran(message);
#endif // CONSOLES_VIRTUELLES

  //   sortirExclusionMutuelle(&emj);
}

void afficherJournal()
{
#ifdef CONSOLES_VIRTUELLES
   montrerConsole(0); // WARNING ! Pas beau le 0 hardcodé
#endif
}
