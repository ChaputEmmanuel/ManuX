/**
 * @file  : main-registre.c
 * @brief : 
 *
 *                                                  (C) Manu Chaput 2024-2025 
 */
#include <manux/config.h>
#include <manux/types.h>     // TRUE
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/memoire.h>
#include <manux/kmalloc.h>
#include <manux/bootloader.h>
#include <manux/registre.h>

#include <manux/debug.h>

/**
 * @brief Exemple de fonction de mise-à-jour d'un paramètre dans un
 * registre 
 */
uint16_t a, b, c;
void miseAJourPararametre(void * prive, char * valeur)
{
   int i = 0;
   uint16_t * n = (uint16_t *)prive;

   printk("Nouvelle valeur = '%s' pour 0x%x\n", valeur, prive);

   // Une conversion bas de gamme
   *n = 0;
   while (valeur[i]) {
      *n = (*n) * 10 + valeur[i++] - '0';
   }
}
			  
void startManuX()
{
   // On doit commencer par ça a priori
   bootloaderInitialiser();
  
   // Initialisation de la console noyau
   consoleInitialisation();

   // On peut initialiser la gestion de la mémoire
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);

   // Initialsation du système kmalloc
   kmallocInitialisation();

   // On initialise le registre
   registreSystemeInitialiser();

   registreSystemeAfficher();

   // Imaginons que la valeur 19 est affectée au paramètre a lors du
   // boot (eg via le bootloader).
   registreSystemeAjouterC("test.a=19");

   // La valeur 42 est affectée au paramètre b, mais d'une autre façon
   // (même résultat).
   registreSystemeAffecterParametre("42", NULL, NULL,
	                            "test", "b", NULL);

   // Maintenant le sous-système test s'initialise. Il a une valeur
   // par défaut pour a, b et c, mais celles de a etb ne seront pas
   // utilisées, puisqu'une valeur a déjà été donnée au boot.
   registreSystemeAffecterParametre("23", &a, miseAJourPararametre,
	                            "test", "a", NULL);

   registreSystemeAffecterParametre("17", &b, miseAJourPararametre,
	                            "test", "b", NULL);

   registreSystemeAffecterParametre("69", &c, miseAJourPararametre,
	                            "test", "c", NULL);

   // Quelques autres paramètres, juste histoire de peupler un
   // registre plus riche
   registreSystemeAffecterParametre("0x483242FF", NULL, NULL,
	                            "systeme", "debug", "mask", NULL);

   registreSystemeAffecterParametre("TRUE", NULL, NULL,
				   "systeme", "debug", "assert", NULL);

   registreSystemeAffecterParametre("192.168.10.2", NULL, NULL, 
				   "reseau", "ip", "eth0", "addr", NULL);

   registreSystemeAffecterParametre("192.168.12.2", NULL, NULL,
				   "reseau", "ip", "eth1", "addr", NULL);

   registreSystemeAfficher();

   printk("Finalement, le systeme va demarrer avec a=%d, b=%d et c=%d\n", a, b, c);

}   /* startManuX */


