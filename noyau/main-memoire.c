/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/debug.h>

/**
 * @brief Structure passée en paramètre par la phase d'init
 * (cf init-manux.nasm)
 */
typedef struct _InfoSysteme {
   uint32_t flags;           // Pour compatibilité avec multiboot
   uint32_t memoireDeBase;   // En Ko
   uint32_t memoireEtendue;  // En Ko
} InfoSysteme;

/**
 * @brief La récupération des variables du script du linker est un peu
 * ésotérique. Voir par exemple
 * https://stackoverflow.com/questions/48561217/how-to-get-value-of-variable-defined-in-ld-linker-script-from-c
 * pour un exemple, et surtout la doc pour une explication 
 * https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
 */
extern uint32_t _adresseDebutManuX[],
                _adresseFinManuX[],
                _adressePileManuX[],
                _adresseLimitePileManuX[];

uint32_t adresseDebutManuX = (uint32_t)_adresseDebutManuX;
uint32_t adresseFinManuX = (uint32_t)_adresseFinManuX;
uint32_t adressePileManuX = (uint32_t)_adressePileManuX;
uint32_t adresseLimitePileManuX = (uint32_t)_adresseLimitePileManuX;

#define NB_PAGES_ALLOC 128

void _startManuX()
{
   InfoSysteme * infoSysteme;
   void * p;

   // Initialisation du pointeur de pile
   __asm__("movl %0,%%esp" : : "a"(adresseLimitePileManuX));

   // Obtention du pointeur vers les informations fournies par l'outil
   // de chargement en mémoire
   __asm__("movl %%ebx,%0" : "=r"(infoSysteme));

   // Initialisation de la console noyau
   consoleInitialisation();

   // Affichage de la mémoire disponible 
   printk("Memoire : %d + %d Ko\n",
	  infoSysteme->memoireDeBase,
	  infoSysteme->memoireEtendue);
   printk("Le noyau va de 0x%x a 0x%x\n",
          adresseDebutManuX, adresseFinManuX);
   printk("La pile actuelle va de 0x%x a 0x%x\n",
          adressePileManuX,
          adresseLimitePileManuX);

   /* Initialisation de la gestion mémoire */
   initialiserMemoire(infoSysteme->memoireDeBase,
		      infoSysteme->memoireEtendue,
		      adresseDebutManuX, adresseFinManuX,
		      adressePileManuX, adresseLimitePileManuX);

   printk("Allocation des pages :\n");
   for (int n = 0; n < NB_PAGES_ALLOC; n++) {
      p = allouerPage();
      printk("0x%x ", p);
   }
   while (1){};   // A remplacer par un halt() 
}   /* _start */

