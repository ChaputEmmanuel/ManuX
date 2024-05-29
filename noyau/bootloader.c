/**
 * @file bootloader.c
 * @brief Gestion de l'interfaçage avec le bootloader
 *
 *                                                     (C) Manu Chaput 2000-2024 
 */
#include <manux/bootloader.h>
#include <manux/debug.h>     // paniqueNoyau
#include <manux/string.h>    // memcpy

#define CMDLINE_MAX_LENGTH 512

static char cmdLine[CMDLINE_MAX_LENGTH];

/**
 * @brief Valeur passée via %eax par le bootloader pour s'identifier
 */ 
uint32_t signatureBootloader;

/**
 * @brief Stockage des informations décrivant le système fournies par
 * le bootloader.
 */
InfoSysteme infoSysteme;

/**
 * @brief Adresse de la zone mémoire dans laquelle le bootloader nous
 * fournit les informations sur le système.
 */
InfoSysteme * _infoSysteme;

/**
 * @brief Lecture des informations fournies par le bootloader
 *
 * Cette fonction doit être invoquée relativement tôt, car elle a la
 * charge de recopier des informations fournies par le bootloader. Ces
 * informations sont dans une zole mémoire que l'on risque ensuite
 * d'utiliser.
 */
void bootloaderInitialiser()
{
   memcpy(&infoSysteme, _infoSysteme, sizeof(InfoSysteme));
  
   // Un petit message
   printk_debug(DBG_KERNEL_BOOTLOADER, "SignatureBootloader = 0x%x, is=0x%x (flags = 0x%x)\n",
	  signatureBootloader,
	  infoSysteme,
	  infoSysteme.flags);

   // Quel est le bootloader ?
   switch (signatureBootloader) {
      case MULTIBOOT_SIGNATURE :
         printk_debug(DBG_KERNEL_BOOTLOADER, "C'est multiboot qui nous a lance.\n");
      break;
      case MANUX_INIT_SIGNATURE :
         printk_debug(DBG_KERNEL_BOOTLOADER, "C'est init-manux qui nous a lance.\n");
      break;
      default :
         paniqueNoyau("Bootloader inconnue : 0x%x\n", signatureBootloader);
   }

   // Alignement des modules à la page ? (Pas important pour nous)
   if ((infoSysteme.flags &  MULTIBOOT_PAGE_ALIGN) ==  MULTIBOOT_PAGE_ALIGN) {
      printk_debug(DBG_KERNEL_BOOTLOADER, "Alignement a la page\n");
   }

   // A-t-on des infos sur la mémoire ?
   if ((infoSysteme.flags & MULTIBOOT_MEMORY_INFO) == MULTIBOOT_MEMORY_INFO) {
      printk_debug(DBG_KERNEL_BOOTLOADER, "Base = %d, etendue = %d\n",
	  infoSysteme.memoireDeBase,
	  infoSysteme.memoireEtendue);
   } else {
      paniqueNoyau("Aucune information sur la memoire !\n");
   }

   // Informations de la ligne de commande (eg GRUB)
   if ((infoSysteme.flags & MULTIBOOT_INFO_CMDLINE) == MULTIBOOT_INFO_CMDLINE) {
      printk_debug(DBG_KERNEL_BOOTLOADER, "Command line : \"%s\"\n", infoSysteme.ligneCommande);
      int l = strlen(infoSysteme.ligneCommande);
      memcpy(cmdLine, infoSysteme.ligneCommande, ((l > 512)?512:l));
      cmdLine[(l > 512)?512:l] = 0;
      printk_debug(DBG_KERNEL_BOOTLOADER, "Command line : \"%s\"\n", infoSysteme.ligneCommande);
   }
}
