ENTRY(start)                    /* Nom du point d'entrée */

SECTIONS {
    . = 0x000020000;            /* C'est ici que tout commence */
    .boot :                     /* On commence par le multiboot */
    {
        *(.multiboot_header)
        *(.multiboot)
    }

    adresseDebutManuX = .;      /* C'est ici que le noyau commence */
    .text : {
       *(.text)
    }        
    .rodata : {
       *(.rodata)
    }
    .data : {
       *(.data)
    }
    .bss : {
       *(.bss)
    }
    adresseFinManuX = .;        /* Repérage de la fin */
}

