/*----------------------------------------------------------------------------*/
/*       Manipulation des entrées/sorties.                                    */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef IO_DEF
#define IO_DEF

/*
 * Emission d'un octet sur un port
 */
#define outb(port, octet) \
   asm ("outb %%al, %1":: "a"(octet), "N"(port))

/*
 * Lecture d'un octet depuis un port
 */
#define inb(port, adresseOctet) \
   asm ("inb %1, %%al": "=a"(adresseOctet): "N"(port))

#endif
