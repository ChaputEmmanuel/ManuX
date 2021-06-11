ENTRY(start)                /* the name of the entry label */

SECTIONS {
    /*. = 1M; */         /* faisons commencer le code ici */
    . = 0x000020000;         /* faisons commencer le code ici */
    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.multiboot)
        *(.multiboot_header)
    }

    .text : { *(.text) }            /* all text sections from all files */
    .data : {
       *(.data)
    }
    .bss : { *(.bss) }
}

