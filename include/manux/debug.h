/*----------------------------------------------------------------------------*/
/*      Ma version des assertions et autres outils de debug.                  */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_DEBUG_DEF
#define MANUX_DEBUG_DEF

#include <manux/config.h>
#include <manux/printk.h>

#define DBG_KERNEL_START 0x00000001

#define masqueDebugage 0xFFFFFFFF

#define printk_debug(lvl, fmt, args...)			 \
   if ((lvl)& masqueDebugage)                    \
     printk("[%10d] %s line %d : %s" fmt,  nbTicks, __FUNCTION__ , __LINE__, ## args)

#endif
