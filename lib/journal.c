/**
 * @file journal.c
 * @brief Implantation des outils de journalisation des messages du noyau.     
 *
 * Il y a des trucs pas très jolis, par exemple l'accès direct à la
 * console. A priori elle devrait être utilisée uniquement via des read/write
 * mais c'est pas plus mal de pouvoir les contourner pour certains
 * debugages. Le truc, c'est que le journal ne la connaît que comme un
 * iNoeud (même s'il sait aller y trouver l'accès direct). Du coup,
 * printk doit passer par le journal, ce qui est bien, mais pour
 * contourner tout ça, j'ai encore un accès direct pas joli dans
 * printk. A supprimer un jour j'espère. Si en particulier la macro
 * MANUX_JOURNAL_DIRECT_CONSOLE ne sert pas trop.
 *
 *                                                  (C) Manu Chaput 2002-2023 
 */
#include <manux/config.h>
#include <manux/memoire.h>  // NULL
#include <manux/journal.h>

/**
 * Le journal utilise la console. Si MANUX_JOURNAL_DIRECT_CONSOLE est
 * activée, il l'accède directement. Sinon il passe par l'interface
 * fichier.
 */
#ifdef MANUX_JOURNAL_DIRECT_CONSOLE
Console * consoleJournal = NULL;
#else
Fichier consoleFichier;
#endif

/**
 * Il faut pourvoir se prémunir si on souhaite utiliser printk avant
 * l'initialisation du journal.
 */
static booleen journalInitialise = FALSE;

/**
 * Le journal utilise également éventuellement un second fichier, par
 * exemple pour envoyer sur l'écran de l'hôte via un virtio-console,
 * ou pour stocker dans un fichier pour analyse post-mortem.
 */
Fichier * fichierJournal = NULL;

//static ExclusionMutuelle emj;

void journalInitialiser(INoeud * iNoeudConsole)
{
    //   initialiserExclusionMutuelle(&emj);
    //   entrerExclusionMutuelle(&emj);

#ifdef MANUX_JOURNAL_DIRECT_CONSOLE
   consoleJournal = iNoeudConsole->prive;
   affecterCouleurTexte(consoleJournal, COUL_TXT_BLANC);

   // Affichons un petit message   
   afficherConsole(consoleJournal, "Journal de ManuX-32\n");
#else
   ouvrirFichier(iNoeudConsole, &consoleFichier);
#endif

   // A partir de maintenant, le journal est opérationnel
   journalInitialise = TRUE;
   
   //   sortirExclusionMutuelle(&emj);
}

void journalAffecterFichier(Fichier * pc)
{
   fichierJournal = pc;
}

/**
 * @brief Journalisation d'un message avec un niveau d'urgence
 */
void journaliserNiveau(booleen console, booleen fichier,
		       uint8_t niveau,
		       char * message, int len)
{
    //   entrerExclusionMutuelle(&emj);

   if ((console) && (niveau <= MANUX_JOURNAL_NIVEAU_DEFAUT)) {
#ifdef MANUX_JOURNAL_DIRECT_CONSOLE
      if (consoleJournal) {
         afficherConsoleN(consoleJournal, message, len);
      } 
#else
      // WARNING, pas de test d'état ! Si j'essaie de journaliser avant
      // la création de la console, ça plante !
      fichierEcrire(&consoleFichier, message, len);   
#endif
   }
   
   if (fichierJournal) {
      if ((fichier) && (niveau <= MANUX_JOURNAL_NIVEAU_DEFAUT)) {
         fichierEcrire(fichierJournal, message, len);   
      }
   }
   
  //   sortirExclusionMutuelle(&emj);
}

/**
 * @brief Gestion des niveaux d'affichage
 */
void aiguillerMessage(char ** message, int * lg,
		      booleen * cons, booleen * fic, uint8_t * niv)
{
   int longueurPrefixe = 0;
   *cons = FALSE;
   *fic = FALSE;
   *niv = 0;


   switch ((*message)[0]) {
      case '[' :
         *cons = TRUE;
      break ;
      case '{' :
         *cons = TRUE;
      case '(' :
         *fic = TRUE;
      break ;
      default :
         *cons = TRUE;  // WARNING defaut
         *fic = TRUE;
      return;
   };

   longueurPrefixe ++;
   if (((*message)[1] <= '9') && ((*message)[1] >= '0')) {
      *niv = (*message)[1] - '0';
      longueurPrefixe ++;

      if (((*message)[2] == ')') || ((*message)[2] == '}') || ((*message)[2] == ']')) {
         longueurPrefixe ++;
      }
   }
   (*message) += longueurPrefixe;
   *lg -= longueurPrefixe;
}

/**
 * @brief Journalisation d'un message
 */
void journaliser(char * message, int len)
{
   booleen console;
   booleen fichier;
   uint8_t niveau;

   aiguillerMessage(&message, &len, &console, &fichier, &niveau);

   journaliserNiveau(console, fichier, niveau, 
		    message, len);
}

/**
 * Pour que printk puisse savoir
 */
booleen journalOperationnel()
{
   return journalInitialise;
}


