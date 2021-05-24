/*----------------------------------------------------------------------------*/
/*      Outils permettant de générer un make.conf pour la compilation de      */
/* ManuX                                                                      */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <stdio.h>

#define addMacro(m)   \
   printf(#m " = 0x%x\n", m); \
  
int main(int argc,char * argv[])
{
   printf("# Fichier généré automatiquement, ne pas modifier\n");

   addMacro(TAILLE_PAGE);
   addMacro(NOMBRE_PAGES_SYSTEME);
   addMacro(KERNEL_START_ADDRESS);

   addMacro(INIT_START_ADDRESS);
   addMacro(MANUX_STACK_SEG_16);

   addMacro(ELF_HEADER_SIZE);
   addMacro(ADRESSE_ECRAN);
   addMacro(NB_SECT_INIT);
   addMacro(NB_SECT_RAMDISK);
   addMacro(SEGMENT_TRANSIT_RAMDISK);
   addMacro(PREMIER_SECT_RAMDISK);

   addMacro(portDonneesClavier);
   addMacro(portCmdClavier);
   
   return 0;
}
