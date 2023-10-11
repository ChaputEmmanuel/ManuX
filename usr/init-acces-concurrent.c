/**
 * @file usr/init.c
 * @brief L'init du monde utilisateur
 * 
 * Attention, ici les include sont dans usr/include !!!
 */
#include <manux/types.h>
#include <stdio.h>
#include <manux/errno.h>
#include <unistd.h>   // creerNouvelleTache
#include <manux/string.h>

/**
 * Taille du buffer utilisé en lecture
 */ 
#define TAILLE_BUFFER 16

int fd[2];  // Le tube

void lecteur()
{
   int r, c=0;
   char b[TAILLE_BUFFER];
   
   printf("Je suis un lecteur !\n");
   fermer(fd[1]);
   
   do {
      r = lire(fd[0], b, TAILLE_BUFFER - 1);
      if (r > 0) {
         b[r] = 0;
         printf(b);
         c += r;
      } else if (r < 0){
	printf("Erreur lecture\n");
      }
   } while (r > 0);

   printf("\nFini ... En tout, j'ai lu %d !\n", c);

   while(1){};
}

void ecrivain()
{
   int r, c  = 0;
   
   char * b = "Bonjour les jeunes ! ";

   printf("Je suis un ecrivain !\n");
   fermer(fd[0]);

   do {
      r = ecrire(fd[1], b, strlen(b));
      if (r >= 0) {
         c += r;
      } else {
         printf("Erreur ecriture\n");
      }
   } while (r > 0);

   printf("En tout, j'ai ecrit %d !\n", c);
}

void init()
{
   int r;

   printf("Bonjour le mode utilisateur !\n");

   r = tube(fd);

   if ( r != ESUCCES) {
      printf("r = %d : casse la pipe !?\n", r);
   } else {
      printf("Tube ok !\n");
      printf("Je lance les taches.\n");
      r = creerNouvelleTache(ecrivain, TRUE);
      r = creerNouvelleTache(lecteur, FALSE);
   }
   while(1){};
}
