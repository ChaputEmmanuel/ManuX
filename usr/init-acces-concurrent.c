/**
 * @file usr/init.c
 * @brief L'init du monde utilisateur
 * 
 * Attention, ici les include sont dans usr/include !!!
 */
#include <manux/types.h>
#include <stdio.h>
#include <unistd.h>   // creerNouvelleTache

void lecteur()
{
   printf("Je suis le lecteur !\n");
   while(1){};
}

void ecrivain()
{
   printf("Je suis l'ecrivain !\n");
   while(1){};
}

void init()
{
   int r;
   int fd[2];  // Le tube

   printf("Sympa le mode utilisateur !\n");

   if (tube(fd) != 0) {
      printf("A casse la pipe !?\n");
   }
   r = creerNouvelleTache(ecrivain, FALSE);
   r = creerNouvelleTache(lecteur, FALSE);
   
   while(1){};
}
