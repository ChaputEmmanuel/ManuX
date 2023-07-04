/**
 * @file usr/init.c
 * @brief L'init du monde utilisateur
 * 
 * Attention, ici les include sont dans usr/include !!!
 */
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>   // creerNouvelleTache

int fd[2];  // Le tube

void lecteur()
{
   int r;
   char b[16];
   
   printf("Je suis le lecteur !\n");
   /*
   r = lire(fd[0], b, 1);

   printf("Je lis %d !\n", r);
   */
   while(1){};
}

void ecrivain()
{
   int r;
   char b[16];

   printf("Je suis l'ecrivain !\n");

   r = ecrire(fd[0], b, 1);

   printf("J'ecris %d !\n", r);

   while(1){};
}

void init()
{
   int r;

   printf("Sympa le mode utilisateur !\n");

   if (tube(fd) != 0 /*ESUCCES*/) {
      printf("A casse la pipe !?\n");
   }
   ecrivain();
   /*
   r = creerNouvelleTache(ecrivain, FALSE);
   r = creerNouvelleTache(lecteur, FALSE);
   */
   while(1){};
}
