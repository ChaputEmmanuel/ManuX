/**
 * @file: noyau/main-test-bloc-ram.c
 * @brief: Un noyau qui teste les supports bloc en RAM
 *
 *   A compiler avec TAILLE_BLOC_RAM définit à 8. On va juste définir
 *   deux supports bloc en RAM qui seront initialisés avec une zone
 *   mémoire qui est une chaîne de caractères de longueur 32 (donc 4
 *   blocs de 8 octets).
 *   On va ensuite effectuer quelques opérations simples en manipulant
 *   ces structures comme  des supports blocs, puis comme des fichier.
 *   
 *                                                          (C) Manu Chaput 2026
 */
#include <manux/config.h>
#include <manux/bootloader.h>
#include <manux/memoire.h>
#include <manux/kmalloc.h>
#include <manux/support-bloc.h>
#include <manux/support-bloc-ram.h>
#include <manux/fichier.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/string.h>

void startManuX(void);

void startManuX(void)
{
   SupportBloc sbRAM1, sbRAM2;
   char * S = "abcdefghijklmnopqrstuvwxyz&!?.:;";
   char * D = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345";
   void * a1, * a2;
   Fichier *f1, *f2;
   char buffer[1024] = {0};
   
   // Initialisation de la console noyau
   consoleInitialisation();
   printk("Test des supports bloc ram ...\n");

   // On a besoin du bootloader pour infoSysteme
   bootloaderInitialiser();
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);

   // On utilise de la mémoire dynamique
   kmallocInitialisation();

   // On initialise les zones mémoires avec les chaînes de caractères
   a1 = (void *) S;
   a2 = (void *) D;

   supportBlocRAMInitialiser(&sbRAM1, a1, strlen(S));
   supportBlocRAMInitialiser(&sbRAM2, a2, strlen(D));
   printk("Supports bloc initialises.\n");

   // 1. - Première phase de tests : lecture et écriture depuis/vers
   // un support bloc

   // 1.1 - On copie depuis a1
   supportBlocLireBlocs(&sbRAM1, 0, 1, buffer);
   
   // 1.2 - On copie vers a2
   supportBlocEcrireBlocs(&sbRAM2, 0, 1, buffer);

   // 1.3 - On copie depuis a1
   supportBlocLireBlocs(&sbRAM1, 1, 3, buffer);
   
   // 1.4 - On copie vers a2
   supportBlocEcrireBlocs(&sbRAM2, 1, 3, buffer);

   // On profite que ce sont des chaînes de caractères
   if (strncmp(D, S, strlen(S)) != 0) {
     printk("Test 1 en erreur !\n");
   } else {
     printk("Test 1 OK !\n");
   }   

   // 2. - Seconde phase de tests : lecture depuis
   // un fichier (spécial de type support bloc)
   f1 = supportBlocOuvrirHorsSF(&sbRAM1);
   for (int i = 0; i < 64;i++) buffer[i] = 0;

   // 2.1 - On commence sur un début de bloc, on finit dans un bloc
   if (fichierLire(f1, buffer, 5) != 5) {
      printk("Erreur lecture 1\n");
   };

   // 2.2 - On commence dans bloc, on finit dans un bloc
   if (fichierLire(f1, buffer+5, 22) != 22) {
      printk("Erreur lecture 2\n");
   };

   // 2.3 - On commence sur un début de bloc, on finit dans un bloc
   if (fichierLire(f1, buffer+5+22, 5) != 5) {
      printk("Erreur lecture 3\n");
   };

   // On profite que ce sont des chaînes de caractères
   if (strncmp(buffer, S, strlen(S)) != 0) {
     printk("Test 2 en erreur !\n");
   } else {
     printk("Test 2 OK !\n");
   }

   // 3. - Troisième phase de tests : écriture vers un ficher (spécial
   // de type support bloc)
   f2 = supportBlocOuvrirHorsSF(&sbRAM2);

   // On a déjà copié S dans D et dans buffer, on va donc écraser par
   // d'autres données
   for (int i = 0; i < 32;i++) buffer[i] = i+42;
   buffer[32] = 0;

   // 3.1 - On commence sur un début de bloc, on finit dans un bloc
   if (fichierEcrire(f2, buffer, 5) != 5) {
      printk("Erreur ecriture 1\n");
   };

   // 3.2 - On commence dans bloc, on finit dans un bloc
   if (fichierEcrire(f2, buffer+5, 22) != 22) {
      printk("Erreur ecriture 2\n");
   };

   // 3.3 - On commence sur un début de bloc, on finit dans un bloc
   if (fichierEcrire(f2, buffer+5+22, 5) != 5) {
      printk("Erreur ecriture 3\n");
   };

   // On profite que ce sont des chaînes de caractères
   if (strncmp(buffer, D, strlen(D)) != 0) {
     printk("Test 3 en erreur !\n");
   } else {
     printk("Test 3 OK !\n");
   }

   printk("Fin du test.\n");

}   // startManuX

