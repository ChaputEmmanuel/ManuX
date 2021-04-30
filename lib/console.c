/*----------------------------------------------------------------------------*/
/*      Implantation des fonctions de base d'accés à la console.              */
/*                                                                            */
/*      Il s'agit ici aussi de quelque chose de très primitif. Chaque         */
/*   console est une zone mémoire de la taille d'un écran et on tape          */
/*   directement là-dedans. Pour rendre une console active, on permutte       */
/*   simplement son adresse avec celle de l'écran physique. Du coup, les      */
/*   affichages se font réellement à l'écran.                                 */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/console.h>

#include <manux/appelsysteme.h>
#include <manux/memoire.h>      /* NULL   */
#include <manux/debug.h>        /* assert */

//#define MANUX_CONSOLE_AVEC_MUTEX

#ifdef CONSOLES_VIRTUELLES

/* A tout moment, il n'y a qu'une console "active" (ie visible) */
Console * consoleActive;

#endif // CONSOLES_VIRTUELLES

/*
 * La console du noyau est celle "par défaut", sur laquelle seront
 * envoyés en particulier les messages du noyau (ceux affichés par
 * printk).
 */
static Console _consoleNoyau;

/*
 * Initialisation du système.
 */
Console * consoleInit()
{
   // L'écran est une zone CON_LIGNESxCON_COLONNES d'adresse fixe
   _consoleNoyau.adresseEcran = CON_SCREEN;
   _consoleNoyau.ligne = 0; //14;
   _consoleNoyau.colonne = 0;
   _consoleNoyau.nbLignes = CON_LIGNES;
   _consoleNoyau.nbColonnes = CON_COLONNES;
   _consoleNoyau.attribut = 15;

#ifdef CONSOLES_VIRTUELLES
   consoleActive = &_consoleNoyau;
   consoleActive->suivante = consoleActive;
   consoleActive->precedente = consoleActive;
#endif

   return &_consoleNoyau;
}


void affecterCouleurFond(Console * cons, Couleur coul)
{
   cons->attribut = (cons->attribut & 0x0F)|(coul<<4);
}

void affecterCouleurTexte(Console * cons, Couleur coul)
{
   cons->attribut = (cons->attribut & 0xF0)|(coul);
}

void effacerConsole(Console * cons)
{
   int l, c;

   for (l = 0; l < cons->nbLignes; l++) {
      for (c = 0; c < cons->nbColonnes; c++) {
         cons->adresseEcran[2*(l*cons->nbColonnes+c)] = ' ';
         cons->adresseEcran[2*(l*cons->nbColonnes+c)+1] = cons->attribut;
      }
   }
   cons->ligne = 0;
   cons->colonne = 0;
}

void scrollUp(Console * cons)
/*
 * Remonté de l'écran d'une ligne
 *
 * WARNING : à réécrire plus efficace
 */
{
    //  static int p = '!';
   int l, c;

   /* Remontée du contenu de l'écran */
   /* A remplacer par bcopy(cons->adresseEcran, cons->adresseEcran+2*cons->nbColonnes, 2*((cons->nbLignes-1)*cons->nbColonnes)) */
   for (l = 0; l < cons->nbLignes - 1; l++) {
      for (c = 0; c < cons->nbColonnes; c++) {
         /* Valeur du caractère */
         cons->adresseEcran[2*(l*cons->nbColonnes+c)] =
            cons->adresseEcran[2*((l+1)*cons->nbColonnes+c)];
         /* Valeur de son attribut */
         cons->adresseEcran[2*(l*cons->nbColonnes+c)+1] =
            cons->adresseEcran[2*((l+1)*cons->nbColonnes+c)+1];
      }
   }
   
   /* On place une ligne d'espaces en bas de l'écran */
   for (c = 0; c < cons->nbColonnes; c++) {
      cons->adresseEcran[2*((cons->nbLignes-1)*cons->nbColonnes+c)] = ' ';
      cons->adresseEcran[2*((cons->nbLignes-1)*cons->nbColonnes+c)+1] = cons->attribut;
   }
}

void avancerLigne(Console * cons)
{
   cons->ligne++;
   if (!(cons->ligne % cons->nbLignes)) {
      cons->ligne = cons->nbLignes - 1;
      scrollUp(cons);
   }
}

void avancerUnCar(Console * cons)
{
   cons->colonne++;
   if (!(cons->colonne % cons->nbColonnes)) {
      cons->colonne = 0;
      avancerLigne(cons);
   }
}

inline void afficherConsoleCaractere(Console * cons, char c)
{
   cons->adresseEcran[(cons->nbColonnes*cons->ligne+cons->colonne)*2] = c;
   cons->adresseEcran[(cons->nbColonnes*cons->ligne+cons->colonne)*2+1] = cons->attribut;
   avancerUnCar(cons);
}

void afficherConsoleN(Console * cons, char * msg, int nbOctets)
{
   int controle;

#ifdef MANUX_CONSOLE_AVEC_MUTEX
   entrerExclusionMutuelle(&cons->scAcces);
#endif

   while (nbOctets) {
      switch (*msg) {
         case '\n' :
            avancerLigne(cons);
            cons->colonne = 0;
         break;
         case ASCII_ESC :
            msg++;
            nbOctets--;
            if (*msg == 91){   // 91 = ASCII('[')
               do {
                  msg++;
                  controle = 0;
                  while((*msg <= '9') && (*msg >= '0')) {
                     controle = controle * 10 + * msg - '0';
                     msg++;nbOctets--;
	          }
                  switch (controle) {
                     case 0 : 
                        affecterCouleurTexte(cons, COUL_TXT_BLANC);
                        affecterCouleurFond(cons, COUL_FOND_NOIR);
                     break;
                     case 30 : 
                        affecterCouleurTexte(cons, COUL_TXT_NOIR);
                     break;
                     case 31 :
                        affecterCouleurTexte(cons, COUL_TXT_ROUGE);
                     break;
                     case 32 : 
                        affecterCouleurTexte(cons, COUL_TXT_VERT);
                     break;
                     case 34 :
                        affecterCouleurTexte(cons, COUL_TXT_BLEU);
                     break;
                     case 37 :
                        affecterCouleurTexte(cons, COUL_TXT_BLANC);
                     break;
                     case 40 : 
                        affecterCouleurFond(cons, COUL_FOND_NOIR);
                     break;
                     case 41 :
                        affecterCouleurFond(cons, COUL_FOND_ROUGE);
                     break;
                     case 42 : 
                        affecterCouleurFond(cons, COUL_FOND_VERT);
                     break;
                     case 44 :
                        affecterCouleurFond(cons, COUL_FOND_BLEU);
                     break;
                     case 47 :
                        affecterCouleurFond(cons, COUL_FOND_GRIS_CLAIR);
                     break;
                     default:
                     break;
		  }
               } while (*msg == 59); // 59 = ASCII(';') 
            }

         break;
         default :
            afficherConsoleCaractere(cons, *msg);
         break;
      }
      msg++;nbOctets--;
   }
#ifdef MANUX_CONSOLE_AVEC_MUTEX
   sortirExclusionMutuelle(&cons->scAcces);
#endif
}

/*
void afficherEcran(char * msg){
   afficherConsole(&_consoleNoyau, msg);
}
*/

void afficherConsole(Console * cons, char * msg)
{
   int n = 0;
   while (msg[n])n++;
   afficherConsoleN(cons, msg, n);
}

void afficherConsoleEntier(Console * cons, int n)
{
   char nombre[60];
   int i = 0;

   do {
      nombre[i] = n%10 + '0';
      n = n / 10;
      i++;
   } while (n);
   for (i--; i>=0; i--) {
      afficherConsoleCaractere(cons, nombre[i]);
   }
}

void afficherConsoleRegistre(Console * cons, int nbOctets, int reg)
{
   char chiffre[17] = "0123456789abcdef";
   char nombre[2*nbOctets];
   int i = 0;

   for (i = 0; i<2*nbOctets; i++) {
      nombre[i] = chiffre[reg%16];
      reg = reg / 16;
   };
   for (i--; i>=0; i--) {
      afficherConsoleCaractere(cons, nombre[i]);
   }
}

#ifdef CONSOLES_VIRTUELLES
/*
 * Initialisation d'une nouvelle console virtuelle. Les espaces
 * mémoire doivent avoir été alloués par ailleurs.
 */
void initialiserConsole(Console * cons, char * adresseEcran)
{
   // Chaque console a sa propre zone mémoire
   cons->adresseEcran = adresseEcran;
   cons->adresseEcranCopie = adresseEcran;

   // La configuration de base
   cons->ligne = 0; 
   cons->colonne = 0;
   cons->nbLignes = CON_LIGNES;
   cons->nbColonnes = CON_COLONNES;
   cons->attribut = 15;

   /* Initialisation du verrou */
#ifdef MANUX_CONSOLE_AVEC_MUTEX
   initialiserExclusionMutuelle(&cons->scAcces);
#endif

   // Un peu de ménage
   effacerConsole(cons);
   
   afficherConsole(cons, "Console [%d] ");   
   afficherConsoleEntier(cons, (int)cons);
   afficherConsole(cons, " creee ...\n");   

   /* On l'insère après la console active dans la liste des consoles gérées */
   cons->suivante = consoleActive->suivante;
   consoleActive->suivante = cons;
   cons->precedente = consoleActive;
   cons->suivante->precedente = cons;
}

/*
 * Basculer vers une console virtuelle. Attention, elle doit exister.
 */
void basculerVersConsoleSuivante()
{
   int i, l, c, a;

   assert(consoleActive != NULL);
   
   if (consoleActive->suivante == consoleActive)
      return;

   assert(consoleActive->suivante != NULL);
   
   // On sauvegarde l'écran physique dans la console active
   for (i=0; i < CON_LIGNES*CON_COLONNES*2; i++) { // WARNING utiliser bopy
      consoleActive->adresseEcranCopie[i] = consoleActive->adresseEcran[i];
   }

   // On la désactive (à partir de maintenant, ce qui y est écrit
   // n'est plus visible à l'écran)
   consoleActive->adresseEcran = consoleActive->adresseEcranCopie;
   
   // On passe à la nouvelle CV 
   consoleActive = consoleActive->suivante;

#ifdef MANUX_CONSOLE_AVEC_MUTEX
   entrerExclusionMutuelle(&consolesVirtuelles[consoleCourante]->scAcces);
#endif

   // On l'active (à partir de maintenant, ce qui y est écrit apparaît
   // directement à l'écran).
   consoleActive->adresseEcran = CON_SCREEN;

   // On copie son état actuel sur l'écran
   for (i=0; i < CON_LIGNES*CON_COLONNES*2; i++) { // WARNING utiliser bopy
      consoleActive->adresseEcran[i] = consoleActive->adresseEcranCopie[i];
   }

   /* On affiche un bandeau d'info en haut */
   l = consoleActive->ligne;
   c = consoleActive->colonne;
   a = consoleActive->attribut;
   consoleActive->ligne = 0;
   consoleActive->colonne = 0;

   consoleActive->attribut = 0x1B;
   afficherConsole(consoleActive, "Cons ");
   afficherConsoleEntier(consoleActive, (int)consoleActive);
   afficherConsole(consoleActive, "  nbTicks ");
   //   afficherConsoleEntier(consoleActive, nbTicks);
   afficherConsole(consoleActive, "            ");

   consoleActive->ligne = l;
   consoleActive->colonne = c;
   consoleActive->attribut = a;
   
#ifdef MANUX_CONSOLE_AVEC_MUTEX
   sortirExclusionMutuelle(&consoleActive->scAcces);
#endif
}

#endif  // CONSOLES_VIRTUELLES

int consoleEcrire(Fichier * f, void * buffer, int nbOctets)
{
   Console * con = f->prive;

   //   printk("Coucou on va balancer sur la console 0x%x (e=0x%x)\n", con, con->adresseEcran);
   afficherConsoleN(con, buffer, nbOctets);
   //printk("Ca y est !");

   //while(1){};

   return nbOctets; // WARNING
}

/*
 * Si l'on n'utilise pas le journal, printk() doit savoir sur quelle
 * console afficher.
 */
Console * consoleNoyau()
{
   return &_consoleNoyau;
};

/*
 * Les méthodes permettant de traiter une console comme un fichier
 */
MethodesFichier consoleMethodesFichier = {
   ecrire : consoleEcrire,
};

