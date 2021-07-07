/*----------------------------------------------------------------------------*/
/*       Manipulation des entrées/sorties.                                    */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef IO_DEF
#define IO_DEF

// WARNING les "Nd" étaient des "N", ...

/*
 * Emission d'un octet sur un port
 */
#define outb(port, octet) \
   asm ("outb %%al, %1":: "a"(octet), "Nd"(port))

/*
 * Lecture d'un octet depuis un port
 */
#define inb(port, adresseOctet) \
   asm ("inb %1, %%al": "=a"(adresseOctet): "Nd"(port))

#endif
