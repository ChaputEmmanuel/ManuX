/**
 * @file usr/init.c
 * @brief L'init du monde utilisateur
 * 
 * Attention, ici les include sont dans usr/include !!!
 */
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>   // creerNouvelleTache
#include <manux/string.h>

int fd[2];  // Le tube

void lecteur()
{
  int r, c=0;
   char b[16];
   
   printf("Je suis le lecteur !\n");

   do {
      r = lire(fd[0], b, 15);
      if (r > 0) {
         b[r] = 0;
         printf(b);
      }
      c += r;
   } while (r > 0);

   printf("J'ai lu %d !\n", c);

}

void ecrivain()
{
   int r, c  = 0;
   
   char * b = "Bonjour les jeunes ! ";

   printf("Je suis l'ecrivain !\n");

   do {
      r = ecrire(fd[0], b, strlen(b));
      c += r;
   } while (r > 0);

   printf("J'ai ecrit %d !\n", c);

}

void init()
{
   int r;

   printf("Sympa le mode utilisateur !\n");

   r = tube(fd);
   if ( r != 0 /*ESUCCES*/) {
      printf("r = %d : casse la pipe !?\n", r);
   }
   
   ecrivain();
   lecteur();
   /*
   r = creerNouvelleTache(ecrivain, FALSE);
   r = creerNouvelleTache(lecteur, FALSE);
   */
}
